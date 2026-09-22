/*
 * Project Ambrose by Imjustchico
 * Tests the panel's own listener: it serves nothing until Panel.Enable is set, it opens its store with the panel tables before it listens, it answers its own routes on a loopback port with its own token, a bind beyond this machine with no certificate is refused with the Panel option names in the message, the plain-HTTP opt-in lifts that refusal, and a certificate and key are served over TLS with the fingerprint the files hold.
 */

#include "AdminClient.h"
#include "ConfigMgr.h"
#include "LogTestDirectory.h"
#include "LogTestHarness.h"
#include "Panel.h"
#include "TlsCertificate.h"

#include <fmt/format.h>

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace
{
    ConfigMgr::EnvironmentLookup NoEnvironment()
    {
        return [](std::string const&) { return std::optional<std::string>(); };
    }

    class PanelTest : public testing::Test
    {
    protected:
        ConfigMgr& Configured(std::string const& text)
        {
            _config = std::make_unique<ConfigMgr>(NoEnvironment());
            EXPECT_TRUE(_config->LoadInitial(_directory.Write("supervisor.conf", text)).Succeeded());
            return *_config;
        }

        Panel Make()
        {
            return Panel(_harness.GetLog(), _directory.Path() / "data", _directory.Path());
        }

        void AddPing(Panel& panel)
        {
            panel.Routes().Add("GET", "/api/panel/ping", [](AdminRequest const&)
            {
                return AdminResponse::Json(200, "{\"pong\":true}");
            });
        }

        LogTestHarness _harness;
        LogTestDirectory _directory;
        std::unique_ptr<ConfigMgr> _config;
    };
}

TEST_F(PanelTest, ServesNothingUntilPanelEnableIsSet)
{
    Panel panel = Make();
    std::string error;
    ASSERT_TRUE(panel.Start(Configured("Panel.Enable = 0\n"), error)) << error;
    EXPECT_FALSE(panel.IsRunning());
    EXPECT_FALSE(panel.Store().IsOpen());
}

TEST_F(PanelTest, OpensItsStoreAndAnswersItsOwnRoutesOnLoopback)
{
    Panel panel = Make();
    AddPing(panel);
    std::string error;
    ASSERT_TRUE(panel.Start(Configured("Panel.Enable = 1\nPanel.Port = 0\n"), error)) << error;
    EXPECT_TRUE(panel.IsRunning());
    EXPECT_FALSE(panel.IsSecure());
    EXPECT_EQ(panel.GetBindIp(), "127.0.0.1");
    ASSERT_NE(panel.GetPort(), 0);

    ASSERT_TRUE(panel.Store().IsOpen());
    EXPECT_TRUE(std::filesystem::exists(_directory.Path() / "data" / "panel" / "panel.sqlite3"));
    std::optional<PanelStore::Statement> tables = panel.Store().Prepare("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name IN ('audit_event', 'audit_subject', 'panel_session')", error);
    ASSERT_TRUE(tables.has_value()) << error;
    ASSERT_TRUE(tables->Step(error)) << error;
    EXPECT_EQ(tables->Int64(0), 3);
    tables.reset();

    AdminClient const anonymous("127.0.0.1", panel.GetPort(), "");
    EXPECT_EQ(anonymous.Send({ "GET", "/api/panel/ping", "", "application/json", "" }, std::chrono::seconds(10)).Status, 401);

    AdminClient const signedIn("127.0.0.1", panel.GetPort(), panel.GetToken());
    AdminClientResponse const answer = signedIn.Send({ "GET", "/api/panel/ping", "", "application/json", "" }, std::chrono::seconds(10));
    ASSERT_TRUE(answer.Answered) << answer.Error;
    EXPECT_EQ(answer.Status, 200) << answer.Body;
    EXPECT_EQ(answer.Body, "{\"pong\":true}");

    panel.Stop();
    EXPECT_FALSE(panel.IsRunning());
    EXPECT_FALSE(panel.Store().IsOpen());
}

TEST_F(PanelTest, RefusesABindBeyondThisMachineInThePanelsOwnOptionNames)
{
    Panel panel = Make();
    std::string error;
    EXPECT_FALSE(panel.Start(Configured("Panel.Enable = 1\nPanel.BindIP = 0.0.0.0\nPanel.Port = 0\n"), error));
    EXPECT_FALSE(panel.IsRunning());
    EXPECT_NE(error.find("Panel.BindIP"), std::string::npos) << error;
    EXPECT_NE(error.find("Panel.CertificateFile"), std::string::npos) << error;
    EXPECT_NE(error.find("Panel.AllowPlainHttpRemote"), std::string::npos) << error;
    EXPECT_NE(error.find("session cookies"), std::string::npos) << error;
    EXPECT_EQ(error.find("Admin."), std::string::npos) << error;

    ListenerSettings const opted = Panel::LoadSettings(Configured("Panel.Enable = 1\nPanel.BindIP = 0.0.0.0\nPanel.AllowPlainHttpRemote = 1\n"));
    EXPECT_FALSE(opted.RemoteAccessError().has_value());
    ASSERT_EQ(opted.Warnings().size(), 1u);
    EXPECT_NE(opted.Warnings().front().find("Panel.AllowPlainHttpRemote"), std::string::npos) << opted.Warnings().front();
}

TEST_F(PanelTest, ServesTlsWithTheCertificateItWasGiven)
{
    std::filesystem::path const certificate = _directory.Path() / "panel.crt";
    std::filesystem::path const key = _directory.Path() / "panel.key";
    std::string error;
    ASSERT_TRUE(TlsCertificate::CreateSelfSigned(certificate, key, "Ambrose panel", { "localhost", "127.0.0.1" }, 60, error)) << error;
    TlsCertificate served;
    ASSERT_TRUE(served.Load(certificate, key, error)) << error;

    Panel panel = Make();
    AddPing(panel);
    ASSERT_TRUE(panel.Start(Configured(fmt::format("Panel.Enable = 1\nPanel.Port = 0\nPanel.CertificateFile = \"{}\"\nPanel.PrivateKeyFile = \"{}\"\n",
        certificate.generic_string(), key.generic_string())), error)) << error;
    EXPECT_TRUE(panel.IsSecure());

    AdminClient const client("127.0.0.1", panel.GetPort(), panel.GetToken(), true);
    AdminClientResponse const answer = client.Send({ "GET", "/api/panel/ping", "", "application/json", "" }, std::chrono::seconds(10));
    ASSERT_TRUE(answer.Answered) << answer.Error;
    EXPECT_EQ(answer.Status, 200) << answer.Body;
    EXPECT_EQ(answer.PeerFingerprint, served.GetInfo().Fingerprint);
    EXPECT_NE(answer.Head.find("Strict-Transport-Security"), std::string::npos) << answer.Head;
}
