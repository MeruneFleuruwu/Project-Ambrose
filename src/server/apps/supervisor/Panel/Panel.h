/*
 * Project Ambrose by Imjustchico
 * The panel's own front door in the supervisor: a second listener with its own Panel options, its own token file and its own store, off unless Panel.Enable is set, holding its operators and their sessions, the counts a failed sign-in adds to, the cost-weighted limit every costly route is held to and the audit tables every change is recorded in, bound to this machine unless a certificate and key are given or the operator opts into plain HTTP, serving the built dashboard at / and the panel's API under /api/panel/, and reloaded with the rest of the configuration so a bind it would not be allowed to keep is refused while the old one goes on serving.
 */

#ifndef AMBROSE_PANEL_H
#define AMBROSE_PANEL_H

#include "AdminServer.h"
#include "ListenerSettings.h"
#include "PanelAudit.h"
#include "PanelRateLimit.h"
#include "PanelSessions.h"
#include "PanelSignIn.h"
#include "PanelUsers.h"
#include "PanelStore.h"
#include "Types.h"

#include <chrono>
#include <filesystem>
#include <map>
#include <utility>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

class ConfigMgr;
class Log;

class Panel
{
public:
    static constexpr uint16 DefaultPort = 12000;
    static constexpr std::string_view Prefix = "/api/panel";
    static constexpr std::chrono::minutes ClaimLifetime{ 30 };

    Panel(Log& log, std::filesystem::path dataFolder, std::filesystem::path configFolder = {});

    Panel(Panel const&) = delete;
    Panel& operator=(Panel const&) = delete;

    static ListenerSettings LoadSettings(ConfigMgr const& config, std::vector<std::string>* problems = nullptr);
    static std::filesystem::path StoreFile(ConfigMgr const& config, std::filesystem::path const& dataFolder);

    bool Start(ConfigMgr const& config, std::string& error);
    bool Reload(ConfigMgr const& config);
    void Stop();

    bool IsRunning() const { return _listener.IsRunning(); }
    uint16 GetPort() const { return _listener.GetPort(); }
    std::string GetBindIp() const { return _listener.GetBindIp(); }
    std::string GetToken() const { return _listener.GetToken(); }
    bool IsSecure() const { return _secure; }
    std::string MintPasswordLink(int64 userId);
    std::string LinkFor(std::string_view token) const;

    PanelStore& Store() { return _store; }
    PanelUsers& Users() { return _users; }
    PanelSessions& Sessions() { return _sessions; }
    PanelSignInThrottle& SignInThrottle() { return _signIn; }
    PanelRateLimit& Limit() { return _rateLimit; }
    AdminRouter& Routes() { return _listener.Routes(); }

    bool Record(AuditEvent const& event, std::function<bool(std::string& error)> const& change, std::string& error);

private:
    bool OpenStore(ConfigMgr const& config, std::string& error);
    void RegisterSignIn();
    void OfferTheOwnerLink();
    AdminResponse Claim(AdminRequest const& request);
    AdminResponse Probe(AdminRequest const& request);
    AdminResponse Reset(AdminRequest const& request);
    AdminResponse OpenFor(PanelUser const& user, AdminRequest const& request, std::string_view how);
    AdminResponse SignIn(AdminRequest const& request);
    AdminResponse SignOut(AdminRequest const& request);
    AdminResponse WhoAmI(AdminRequest const& request);
    std::optional<PanelUser> UserOf(AdminRequest const& request);
    std::optional<AdminResponse> Throttle(AdminRequest const& request, uint32 cost);

    Log& _log;
    std::filesystem::path _dataFolder;
    PanelStore _store;
    PanelUsers _users;
    PanelSessions _sessions;
    PanelSignInThrottle _signIn;
    std::mutex _claimMutex;
    std::string _claimToken;
    std::chrono::steady_clock::time_point _claimExpires;
    std::chrono::seconds _sessionIdle{ 0 };
    std::chrono::seconds _sessionLifetime{ 0 };
    std::map<std::string, std::pair<int64, std::chrono::steady_clock::time_point>> _resets;
    PanelRateLimit _rateLimit;
    std::mutex _storeMutex;
    AdminServer _listener;
    bool _secure = false;
};

#endif
