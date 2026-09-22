<!-- Project Ambrose by Imjustchico: The panel frame: the side bar built from the route table, the page the address names after its hash, a sample-data notice, and the access-denied page for a route the viewer may not use. -->
<script lang="ts">
    import { AppShell, Badge, Button, SideNav } from "@ambrose/ui";
    import { findRoute, routes } from "./routes";
    import Overview from "./pages/Overview.svelte";
    import Servers from "./pages/Servers.svelte";
    import Logs from "./pages/Logs.svelte";
    import Console from "./pages/Console.svelte";
    import Lists from "./pages/Lists.svelte";
    import ClientData from "./pages/ClientData.svelte";
    import Settings from "./pages/Settings.svelte";
    import Denied from "./pages/Denied.svelte";

    const granted = new Set(routes.map((route) => route.permission));

    function readPath(): string {
        const path = window.location.hash.replace(/^#\/?/, "");
        return path === "" ? "overview" : path;
    }

    let path = $state(readPath());

    $effect(() => {
        const follow = () => (path = readPath());
        window.addEventListener("hashchange", follow);
        return () => window.removeEventListener("hashchange", follow);
    });

    const route = $derived(findRoute(path));
    const allowed = $derived(route !== undefined && (route.permission === "none" || granted.has(route.permission)));

    const groups = (["Servers", "Game", "Panel"] as const).map((heading) => ({
        heading,
        entries: routes.filter((entry) => entry.nav && entry.group === heading && granted.has(entry.permission)).map((entry) => ({ href: `#${entry.path}`, label: entry.title, icon: entry.icon })),
    }));
</script>

<AppShell product="Ambrose">
    {#snippet side()}
        <SideNav label="Panel sections" {groups} current={`#${path}`} />
    {/snippet}
    {#snippet barEnd()}
        <Badge tone="waiting">Sample data</Badge>
        <Button variant="quiet" icon="log-out">Sign out</Button>
    {/snippet}
    {#if !route}
        <Denied title="No such page" detail={`Nothing in the panel lives at #${path}.`} />
    {:else if !allowed}
        <Denied title="Access denied" detail={`${route.title} needs the ${route.permission} permission, which your role does not grant.`} />
    {:else if route.path === "overview"}
        <Overview />
    {:else if route.path === "servers"}
        <Servers />
    {:else if route.path === "logs"}
        <Logs />
    {:else if route.path === "console"}
        <Console />
    {:else if route.path === "client"}
        <ClientData />
    {:else if route.path === "settings"}
        <Settings />
    {:else if route.path === "denied"}
        <Denied title="Access denied" detail="This page is where a link you may not follow lands." />
    {:else}
        <Lists which={route.path} title={route.title} />
    {/if}
</AppShell>
