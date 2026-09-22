/*
 * Project Ambrose by Imjustchico
 * The panel's route table: every page with its path, title, icon, the permission it needs and whether it appears in the side bar, grouped the way the side bar shows them.
 */

import type { IconName } from "@ambrose/ui";

export type Route = {
    path: string;
    title: string;
    icon: IconName;
    permission: string;
    nav: boolean;
    group: "Servers" | "Game" | "Panel";
};

export const routes: Route[] = [
    { path: "overview", title: "Overview", icon: "gauge", permission: "status.read", nav: true, group: "Servers" },
    { path: "servers", title: "Servers", icon: "server", permission: "apps.read", nav: true, group: "Servers" },
    { path: "logs", title: "Logs", icon: "file-text", permission: "logs.read", nav: true, group: "Servers" },
    { path: "console", title: "Console", icon: "terminal", permission: "commands.run", nav: true, group: "Servers" },
    { path: "backups", title: "Backups", icon: "archive", permission: "backups.read", nav: true, group: "Servers" },
    { path: "realms", title: "Realms and zones", icon: "wifi", permission: "realms.read", nav: true, group: "Game" },
    { path: "players", title: "Players online", icon: "user", permission: "players.read", nav: true, group: "Game" },
    { path: "accounts", title: "Accounts and bans", icon: "shield", permission: "accounts.read", nav: true, group: "Game" },
    { path: "client", title: "Client data", icon: "hard-drive", permission: "client.read", nav: true, group: "Game" },
    { path: "users", title: "Panel users", icon: "users", permission: "panel.users.read", nav: true, group: "Panel" },
    { path: "settings", title: "Settings", icon: "settings", permission: "settings.read", nav: true, group: "Panel" },
    { path: "activity", title: "Activity", icon: "activity", permission: "audit.read", nav: true, group: "Panel" },
    { path: "denied", title: "Access denied", icon: "lock", permission: "none", nav: false, group: "Panel" },
];

export function findRoute(path: string): Route | undefined {
    return routes.find((route) => route.path === path);
}
