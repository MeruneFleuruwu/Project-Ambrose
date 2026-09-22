<!-- Project Ambrose by Imjustchico: The list pages that are one table and a few actions each: players online, accounts and bans, realms, backups, panel users and activity. -->
<script lang="ts">
    import { Button, DataTable, Heading, TextField } from "@ambrose/ui";
    import { accounts, activity, backups, panelUsers, players, realms } from "../sample";

    type Props = { which: string; title: string };
    let { which, title }: Props = $props();

    type Row = Record<string, string>;
    type Page = { caption: string; rows: Row[]; columns: [string, string][]; mono?: string[]; actions: { label: string; icon: "plus" | "download" | "refresh-cw" | "user" | "shield" }[]; search?: string };

    const pages: Record<string, Page> = {
        players: { caption: "Wizards in the world now", rows: players, columns: [["name", "Wizard"], ["level", "Level"], ["school", "School"], ["zone", "Zone"], ["session", "Online for"], ["realm", "Realm"]], actions: [{ label: "Message everyone", icon: "user" }], search: "Find a wizard" },
        accounts: { caption: "Game accounts", rows: accounts, columns: [["username", "Account"], ["created", "Created"], ["lastSeen", "Last seen"], ["level", "Security level"], ["state", "State"]], actions: [{ label: "New account", icon: "plus" }, { label: "Ban", icon: "shield" }], search: "Find an account by name, address or machine" },
        realms: { caption: "Realms in the realm list", rows: realms, columns: [["name", "Realm"], ["address", "Address"], ["flags", "Flags"], ["population", "Population"], ["heartbeat", "Heartbeat"], ["zones", "Zones loaded"]], mono: ["address"], actions: [{ label: "Refresh", icon: "refresh-cw" }] },
        backups: { caption: "Backups kept on this machine", rows: backups, columns: [["name", "Backup"], ["taken", "Taken"], ["size", "Size"], ["verified", "Check"], ["kept", "Kept for"]], mono: ["name"], actions: [{ label: "Back up now", icon: "plus" }, { label: "Download", icon: "download" }] },
        users: { caption: "People who can sign in to this panel", rows: panelUsers, columns: [["name", "User"], ["role", "Role"], ["twoFactor", "Two-factor"], ["lastSignIn", "Last sign-in"]], actions: [{ label: "Invite", icon: "plus" }] },
        activity: { caption: "What people did in the panel", rows: activity, columns: [["when", "When"], ["who", "Who"], ["what", "What"], ["where", "Where"]], actions: [{ label: "Export", icon: "download" }] },
    };

    const page = $derived(pages[which]);
    const columns = $derived(page ? page.columns.map(([id, header]) => ({ id, header, value: (row: Row) => row[id] ?? "", sortable: true, mono: page.mono?.includes(id) ?? false })) : []);
</script>

<div class="flex flex-col gap-20">
    <div class="flex flex-wrap items-center justify-between gap-12">
        <Heading level={1}>{title}</Heading>
        {#if page}
            <div class="flex flex-wrap gap-8">
                {#each page.actions as action, index (action.label)}
                    <Button variant={index === 0 ? "action" : "quiet"} icon={action.icon}>{action.label}</Button>
                {/each}
            </div>
        {/if}
    </div>
    {#if page}
        {#if page.search}
            <TextField id={`${which}-search`} label="Search" type="search" placeholder={page.search} class="max-w-480" />
        {/if}
        <DataTable caption={page.caption} {columns} rows={page.rows} />
    {/if}
</div>
