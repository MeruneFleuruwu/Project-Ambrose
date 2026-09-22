/*
 * Project Ambrose by Imjustchico
 * Reads the Panel options into a listener of the same shape as an app's admin API, opens the store before the listener so nothing serves without somewhere to write, names the certificate and key in Panel option names when the bind rule refuses them, and starts, reloads and stops the listener beside the supervisor's own; a reload that would leave the bind unsafe or the certificate unservable is refused and the old listener keeps serving.
 */

#include "Panel.h"
#include "ConfigMgr.h"
#include "Log.h"
#include "SourceFolder.h"
#include "StringUtil.h"

#include <fmt/format.h>

#include <mutex>
#include <utility>

namespace
{
    constexpr char const* PanelCategory = "server.panel";

    std::filesystem::path Configured(ConfigMgr const& config, std::string const& key, std::filesystem::path const& fallback)
    {
        std::filesystem::path const value = ConfigMgr::PathFromUtf8(Ambrose::Trim(config.GetOption<std::string>(key, "", true)));
        return value.empty() ? fallback : value.lexically_normal();
    }
}

Panel::Panel(Log& log, std::filesystem::path dataFolder, std::filesystem::path configFolder)
    : _log(log), _dataFolder(std::move(dataFolder)), _listener(log, "panel", _dataFolder, std::move(configFolder))
{
    _listener.Routes().SetThrottle([this](AdminRequest const& request, uint32 cost) { return Throttle(request, cost); });
}

ListenerSettings Panel::LoadSettings(ConfigMgr const& config, std::vector<std::string>* problems)
{
    ListenerSettings settings = ListenerSettings::Load(config, "Panel", DefaultPort, problems);
    settings.Label = "the panel";
    settings.LogCategory = PanelCategory;
    settings.Secrets = "the token, sign-in passwords, two-factor codes and session cookies";
    return settings;
}

std::filesystem::path Panel::StoreFile(ConfigMgr const& config, std::filesystem::path const& dataFolder)
{
    std::filesystem::path const home = (dataFolder.empty() ? config.GetFilename().parent_path() : dataFolder) / "panel";
    return Configured(config, "Panel.StoreFile", home / "panel.sqlite3");
}

bool Panel::OpenStore(ConfigMgr const& config, std::string& error)
{
    std::lock_guard const lock(_storeMutex);
    if (_store.IsOpen())
        return true;
    std::vector<std::string> warnings;
    std::filesystem::path const file = StoreFile(config, _dataFolder);
    if (!_store.Open(file, Ambrose::FindSourceFolder(), warnings, error))
        return false;
    for (std::string const& warning : warnings)
        AMBROSE_LOG(_log, LogLevel::Warn, PanelCategory, "{}", warning);
    for (std::string const& applied : _store.GetApplied())
        AMBROSE_LOG(_log, LogLevel::Info, PanelCategory, "The panel store applied {}", applied);
    AMBROSE_LOG(_log, LogLevel::Info, PanelCategory, "The panel store is open in {}", ConfigMgr::PathToUtf8(file));
    return true;
}

bool Panel::Start(ConfigMgr const& config, std::string& error)
{
    std::vector<std::string> problems;
    ListenerSettings const settings = LoadSettings(config, &problems);
    for (std::string const& problem : problems)
        AMBROSE_LOG(_log, LogLevel::Warn, PanelCategory, "{}", problem);
    if (!settings.Enable)
    {
        AMBROSE_LOG(_log, LogLevel::Info, PanelCategory, "Panel.Enable = 0, so the panel serves nothing of its own; the supervisor's admin API still serves it");
        return true;
    }
    if (!OpenStore(config, error))
        return false;
    _rateLimit.SetLimits(settings.RateLimitBurst, settings.RateLimitPerSecond);
    _secure = settings.HasTls();
    return _listener.Start(settings, error);
}

bool Panel::Reload(ConfigMgr const& config)
{
    std::vector<std::string> problems;
    ListenerSettings const settings = LoadSettings(config, &problems);
    for (std::string const& problem : problems)
        AMBROSE_LOG(_log, LogLevel::Warn, PanelCategory, "{}", problem);
    if (settings.Enable && !_store.IsOpen())
    {
        std::string error;
        if (!OpenStore(config, error))
        {
            AMBROSE_LOG(_log, LogLevel::Error, PanelCategory, "The panel store could not be opened, so the panel stays as it is: {}", error);
            return false;
        }
    }
    _rateLimit.SetLimits(settings.RateLimitBurst, settings.RateLimitPerSecond);
    if (!_listener.Reload(settings))
        return false;
    _secure = settings.Enable && settings.HasTls();
    return true;
}

void Panel::Stop()
{
    _listener.Stop();
    _rateLimit.Clear();
    std::lock_guard const lock(_storeMutex);
    _store.Close();
    _secure = false;
}

bool Panel::Record(AuditEvent const& event, std::function<bool(std::string& error)> const& change, std::string& error)
{
    std::lock_guard const lock(_storeMutex);
    if (!_store.IsOpen())
    {
        error = "the panel store is not open, so nothing can be recorded and nothing is changed";
        return false;
    }
    return PanelAudit::Record(_store, event, change, error);
}

std::optional<AdminResponse> Panel::Throttle(AdminRequest const& request, uint32 cost)
{
    if (cost == 0)
        return std::nullopt;
    PanelRateVerdict const verdict = _rateLimit.Take(request.Principal, request.RemoteAddress, cost);
    if (verdict.Allowed)
        return std::nullopt;
    if (verdict.FirstThisMinute)
    {
        AuditEvent event;
        event.Name = "panel:request.throttled";
        event.Actor = request.Principal == "token" ? AuditActor::Token : AuditActor::User;
        event.ActorId = request.Principal;
        event.Address = request.RemoteAddress;
        event.Result = AuditResult::Throttled;
        event.Reason = fmt::format("{} {} costs {}, which is more than the {} bucket has left", request.Method, request.Path, cost, verdict.Bucket);
        event.Properties = fmt::format(R"({{"method":"{}","path":"{}","cost":{},"bucket":"{}","retry_after":{}}})",
            request.Method, request.Path, cost, verdict.Bucket, verdict.RetryAfterSeconds);
        event.On("route", fmt::format("{} {}", request.Method, request.Path));
        std::string error;
        if (!Record(event, {}, error))
            AMBROSE_LOG(_log, LogLevel::Error, PanelCategory, "A throttled request could not be recorded: {}", error);
    }
    AdminResponse held = AdminResponse::Problem(429, "too_many_requests",
        fmt::format("The panel is holding this request back; try again in {} second{}", verdict.RetryAfterSeconds, verdict.RetryAfterSeconds == 1 ? "" : "s"));
    held.Headers.emplace_back("Retry-After", std::to_string(verdict.RetryAfterSeconds));
    return held;
}
