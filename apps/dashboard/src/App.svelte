<!-- Project Ambrose by Imjustchico: The panel frame on the shadcn-svelte sidebar: the sections built from the route table, collapsible to icons on a desktop and a drawer on a phone, a top bar with the breadcrumb and the signed-in user's menu, the page the address names after its hash, and the access-denied page for a route the viewer may not use. -->
<script lang="ts">
    import * as Avatar from "$lib/components/ui/avatar/index.js";
    import * as Breadcrumb from "$lib/components/ui/breadcrumb/index.js";
    import * as DropdownMenu from "$lib/components/ui/dropdown-menu/index.js";
    import * as Sidebar from "$lib/components/ui/sidebar/index.js";
    import { Badge } from "$lib/components/ui/badge/index.js";
    import { Separator } from "$lib/components/ui/separator/index.js";
    import ChevronsUpDownIcon from "@lucide/svelte/icons/chevrons-up-down";
    import LogOutIcon from "@lucide/svelte/icons/log-out";
    import SparklesIcon from "@lucide/svelte/icons/sparkles";
    import UserCogIcon from "@lucide/svelte/icons/user-cog";
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
        entries: routes.filter((entry) => entry.nav && entry.group === heading && granted.has(entry.permission)),
    }));
</script>

<Sidebar.Provider>
    <Sidebar.Root collapsible="icon" variant="inset">
        <Sidebar.Header>
            <Sidebar.Menu>
                <Sidebar.MenuItem>
                    <Sidebar.MenuButton size="lg">
                        {#snippet child({ props })}
                            <a href="#overview" {...props}>
                                <div class="flex aspect-square size-8 items-center justify-center rounded-lg bg-primary font-serif text-lg font-bold text-primary-foreground">A</div>
                                <div class="grid flex-1 text-left leading-tight">
                                    <span class="truncate font-serif text-base font-semibold">Ambrose</span>
                                    <span class="truncate text-xs text-muted-foreground">Server panel</span>
                                </div>
                            </a>
                        {/snippet}
                    </Sidebar.MenuButton>
                </Sidebar.MenuItem>
            </Sidebar.Menu>
        </Sidebar.Header>
        <Sidebar.Content>
            {#each groups as group (group.heading)}
                <Sidebar.Group>
                    <Sidebar.GroupLabel>{group.heading}</Sidebar.GroupLabel>
                    <Sidebar.GroupContent>
                        <Sidebar.Menu>
                            {#each group.entries as entry (entry.path)}
                                <Sidebar.MenuItem>
                                    <Sidebar.MenuButton isActive={entry.path === path} tooltipContent={entry.title}>
                                        {#snippet child({ props })}
                                            <a href={`#${entry.path}`} {...props}>
                                                <entry.icon />
                                                <span>{entry.title}</span>
                                            </a>
                                        {/snippet}
                                    </Sidebar.MenuButton>
                                </Sidebar.MenuItem>
                            {/each}
                        </Sidebar.Menu>
                    </Sidebar.GroupContent>
                </Sidebar.Group>
            {/each}
        </Sidebar.Content>
        <Sidebar.Footer>
            <Sidebar.Menu>
                <Sidebar.MenuItem>
                    <DropdownMenu.Root>
                        <DropdownMenu.Trigger>
                            {#snippet child({ props })}
                                <Sidebar.MenuButton size="lg" {...props}>
                                    <Avatar.Root class="size-8 rounded-lg">
                                        <Avatar.Fallback class="rounded-lg">IC</Avatar.Fallback>
                                    </Avatar.Root>
                                    <div class="grid flex-1 text-left text-sm leading-tight">
                                        <span class="truncate font-medium">Imjustchico</span>
                                        <span class="truncate text-xs text-muted-foreground">Owner</span>
                                    </div>
                                    <ChevronsUpDownIcon class="ml-auto size-4" />
                                </Sidebar.MenuButton>
                            {/snippet}
                        </DropdownMenu.Trigger>
                        <DropdownMenu.Content class="w-56 rounded-lg" side="right" align="end" sideOffset={4}>
                            <DropdownMenu.Label class="font-normal">
                                <div class="text-sm font-medium">Imjustchico</div>
                                <div class="text-xs text-muted-foreground">Owner of this panel</div>
                            </DropdownMenu.Label>
                            <DropdownMenu.Separator />
                            <DropdownMenu.Item><UserCogIcon />Account and security</DropdownMenu.Item>
                            <DropdownMenu.Item><SparklesIcon />What's new</DropdownMenu.Item>
                            <DropdownMenu.Separator />
                            <DropdownMenu.Item><LogOutIcon />Sign out</DropdownMenu.Item>
                        </DropdownMenu.Content>
                    </DropdownMenu.Root>
                </Sidebar.MenuItem>
            </Sidebar.Menu>
        </Sidebar.Footer>
        <Sidebar.Rail />
    </Sidebar.Root>
    <Sidebar.Inset>
        <header class="flex h-14 shrink-0 items-center gap-2 border-b px-4">
            <Sidebar.Trigger class="-ml-1" />
            <Separator orientation="vertical" class="mr-2 data-[orientation=vertical]:h-4" />
            <Breadcrumb.Root>
                <Breadcrumb.List>
                    <Breadcrumb.Item class="hidden md:block">
                        <Breadcrumb.Link href="#overview">Ambrose</Breadcrumb.Link>
                    </Breadcrumb.Item>
                    <Breadcrumb.Separator class="hidden md:block" />
                    <Breadcrumb.Item>
                        <Breadcrumb.Page>{route?.title ?? "Not found"}</Breadcrumb.Page>
                    </Breadcrumb.Item>
                </Breadcrumb.List>
            </Breadcrumb.Root>
            <Badge variant="outline" class="ml-auto gap-1.5 border-waiting/40 bg-waiting/10 text-waiting">
                <span class="size-1.5 rounded-full bg-waiting"></span>
                Sample data
            </Badge>
        </header>
        <main class="flex flex-1 flex-col gap-6 p-4 md:p-6">
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
        </main>
    </Sidebar.Inset>
</Sidebar.Provider>
