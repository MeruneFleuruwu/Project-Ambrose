/*
 * Project Ambrose by Imjustchico
 * The panel's own front door in the supervisor: a second listener with its own Panel options, its own token file and its own store, off unless Panel.Enable is set, holding the cost-weighted limit every costly route is held to and the audit tables every change is recorded in, bound to this machine unless a certificate and key are given or the operator opts into plain HTTP, serving the built dashboard at / and the panel's API under /api/panel/, and reloaded with the rest of the configuration so a bind it would not be allowed to keep is refused while the old one goes on serving.
 */

#ifndef AMBROSE_PANEL_H
#define AMBROSE_PANEL_H

#include "AdminServer.h"
#include "ListenerSettings.h"
#include "PanelAudit.h"
#include "PanelRateLimit.h"
#include "PanelStore.h"
#include "Types.h"

#include <filesystem>
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
    PanelStore& Store() { return _store; }
    PanelRateLimit& Limit() { return _rateLimit; }
    AdminRouter& Routes() { return _listener.Routes(); }

    bool Record(AuditEvent const& event, std::function<bool(std::string& error)> const& change, std::string& error);

private:
    bool OpenStore(ConfigMgr const& config, std::string& error);
    std::optional<AdminResponse> Throttle(AdminRequest const& request, uint32 cost);

    Log& _log;
    std::filesystem::path _dataFolder;
    PanelStore _store;
    PanelRateLimit _rateLimit;
    std::mutex _storeMutex;
    AdminServer _listener;
    bool _secure = false;
};

#endif
