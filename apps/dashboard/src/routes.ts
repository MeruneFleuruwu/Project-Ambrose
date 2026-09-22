/*
 * Project Ambrose by Imjustchico
 * The panel's route table: every page with its path, title, icon, the permission it needs, whether it appears in the side bar, the group it shows under, and how it is drawn: its own page loaded on first visit, a list page, or the access-denied page.
 */

import type { Component } from "svelte";
import ActivityIcon from "@lucide/svelte/icons/activity";
import ArchiveIcon from "@lucide/svelte/icons/archive";
import FileTextIcon from "@lucide/svelte/icons/file-text";
import GaugeIcon from "@lucide/svelte/icons/gauge";
import GlobeIcon from "@lucide/svelte/icons/globe";
import HardDriveIcon from "@lucide/svelte/icons/hard-drive";
import LockIcon from "@lucide/svelte/icons/lock";
import ServerIcon from "@lucide/svelte/icons/server";
import SettingsIcon from "@lucide/svelte/icons/settings";
import ShieldIcon from "@lucide/svelte/icons/shield";
import SquareTerminalIcon from "@lucide/svelte/icons/square-terminal";
import UserIcon from "@lucide/svelte/icons/user";
import UsersIcon from "@lucide/svelte/icons/users";

export type Route = {
    path: string;
    title: string;
    icon: Component;
    permission: string;
    nav: boolean;
    group: "Servers" | "Game" | "Panel";
    page: (() => Promise<{ default: Component }>) | "list" | "denied";
};

export const routes: Route[] = [
    {
        path: "overview",
        title: "Overview",
        icon: GaugeIcon,
        permission: "status.read",
        nav: true,
        group: "Servers",
        page: () => import("./pages/Overview.svelte"),
    },
    {
        path: "servers",
        title: "Servers",
        icon: ServerIcon,
        permission: "apps.read",
        nav: true,
        group: "Servers",
        page: () => import("./pages/Servers.svelte"),
    },
    {
        path: "logs",
        title: "Logs",
        icon: FileTextIcon,
        permission: "logs.read",
        nav: true,
        group: "Servers",
        page: () => import("./pages/Logs.svelte"),
    },
    {
        path: "console",
        title: "Console",
        icon: SquareTerminalIcon,
        permission: "commands.run",
        nav: true,
        group: "Servers",
        page: () => import("./pages/Console.svelte"),
    },
    { path: "backups", title: "Backups", icon: ArchiveIcon, permission: "backups.read", nav: true, group: "Servers", page: "list" },
    { path: "realms", title: "Realms and zones", icon: GlobeIcon, permission: "realms.read", nav: true, group: "Game", page: "list" },
    { path: "players", title: "Players online", icon: UserIcon, permission: "players.read", nav: true, group: "Game", page: "list" },
    { path: "accounts", title: "Accounts and bans", icon: ShieldIcon, permission: "accounts.read", nav: true, group: "Game", page: "list" },
    {
        path: "client",
        title: "Client data",
        icon: HardDriveIcon,
        permission: "client.read",
        nav: true,
        group: "Game",
        page: () => import("./pages/ClientData.svelte"),
    },
    { path: "users", title: "Panel users", icon: UsersIcon, permission: "panel.users.read", nav: true, group: "Panel", page: "list" },
    {
        path: "settings",
        title: "Settings",
        icon: SettingsIcon,
        permission: "settings.read",
        nav: true,
        group: "Panel",
        page: () => import("./pages/Settings.svelte"),
    },
    { path: "activity", title: "Activity", icon: ActivityIcon, permission: "audit.read", nav: true, group: "Panel", page: "list" },
    { path: "denied", title: "Access denied", icon: LockIcon, permission: "none", nav: false, group: "Panel", page: "denied" },
];

export function findRoute(path: string): Route | undefined {
    return routes.find((route) => route.path === path);
}
