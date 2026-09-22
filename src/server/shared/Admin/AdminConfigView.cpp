/*
 * Project Ambrose by Imjustchico
 * Builds the settings answer from the config's own layers: every loaded key in order with its effective and shipped values, where each was read, the restart reason the app declared for it or none, and the secrets masked the same way the log stream masks them.
 */

#include "AdminConfigView.h"
#include "AdminRouter.h"
#include "LogRedaction.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <optional>

std::string_view AdminConfigView::LayerName(ConfigSourceKind kind) noexcept
{
    switch (kind)
    {
        case ConfigSourceKind::Default: return "default";
        case ConfigSourceKind::ModuleDefault: return "module_default";
        case ConfigSourceKind::Config: return "config";
        case ConfigSourceKind::ModuleConfig: return "module_config";
        case ConfigSourceKind::Environment: return "environment";
        case ConfigSourceKind::Override: return "override";
    }
    return "config";
}

std::string AdminConfigView::SettingsJson(ConfigMgr const& config, std::span<RestartRequiredOption const> restartRequired)
{
    nlohmann::json settings = nlohmann::json::array();
    for (std::string const& key : config.GetKeysByString(""))
    {
        std::optional<ConfigEntry> const effective = config.Resolve(key);
        if (!effective)
            continue;
        bool const secret = LogRedaction::IsSecretSetting(key);
        nlohmann::json entry;
        entry["key"] = key;
        entry["value"] = LogRedaction::RedactSettingValue(key, effective->Value);
        entry["layer"] = LayerName(effective->Kind);
        entry["file"] = ConfigMgr::PathToUtf8(effective->File);
        entry["line"] = effective->Line;
        if (std::optional<ConfigEntry> const shipped = config.ResolveDefault(key))
        {
            entry["default"] = LogRedaction::RedactSettingValue(key, shipped->Value);
            entry["default_file"] = ConfigMgr::PathToUtf8(shipped->File);
        }
        else
        {
            entry["default"] = nullptr;
            entry["default_file"] = nullptr;
        }
        entry["secret"] = secret;
        auto const restart = std::ranges::find(restartRequired, std::string_view(key), &RestartRequiredOption::Key);
        entry["restart_reason"] = restart == restartRequired.end() ? nlohmann::json(nullptr) : nlohmann::json(std::string(restart->Reason));
        settings.push_back(std::move(entry));
    }
    nlohmann::json body;
    body["schema"] = SchemaVersion;
    body["file"] = ConfigMgr::PathToUtf8(config.GetFilename());
    body["settings"] = std::move(settings);
    return body.dump();
}

void AdminConfigView::Register(AdminRouter& router, ConfigMgr const& config, std::vector<RestartRequiredOption> restartRequired)
{
    router.Add("GET", "/api/settings", [&config, restartRequired = std::move(restartRequired)](AdminRequest const&) { return AdminResponse::Json(200, SettingsJson(config, restartRequired)); });
}
