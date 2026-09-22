<!-- Project Ambrose by Imjustchico: The panel frame on the shadcn-svelte sidebar: the sections built from the route table, collapsible to icons on a desktop and a drawer on a phone, a top bar with the breadcrumb and the signed-in user's menu, a search button that opens the command palette, the light and dark choice in the user's menu, toasts in the corner, the page the address names after its hash with its code fetched on first visit, and the access-denied page for a route the viewer may not use. -->
<script lang="ts">
    import * as Avatar from "$lib/components/ui/avatar/index.js";
    import * as Breadcrumb from "$lib/components/ui/breadcrumb/index.js";
    import * as DropdownMenu from "$lib/components/ui/dropdown-menu/index.js";
    import * as Kbd from "$lib/components/ui/kbd/index.js";
    import * as Sidebar from "$lib/components/ui/sidebar/index.js";
    import { Badge } from "$lib/components/ui/badge/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import { Separator } from "$lib/components/ui/separator/index.js";
    import { Skeleton } from "$lib/components/ui/skeleton/index.js";
    import { Toaster } from "$lib/components/ui/sonner/index.js";
    import { chooseTheme, theme, type ThemeChoice } from "$lib/theme.svelte.js";
    import ChevronsUpDownIcon from "@lucide/svelte/icons/chevrons-up-down";
    import LogOutIcon from "@lucide/svelte/icons/log-out";
    import RotateCcwIcon from "@lucide/svelte/icons/rotate-ccw";
    import SearchIcon from "@lucide/svelte/icons/search";
    import SparklesIcon from "@lucide/svelte/icons/sparkles";
    import UserCogIcon from "@lucide/svelte/icons/user-cog";
    import CommandPalette from "./components/CommandPalette.svelte";
    import { findRoute, routes } from "./routes";
    import Lists from "./pages/Lists.svelte";
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
    const reachable = routes.filter((entry) => entry.nav && granted.has(entry.permission));
    const groups = (["Servers", "Game", "Panel"] as const).map((heading) => ({
        heading,
        entries: reachable.filter((entry) => entry.group === heading),
    }));
    const onMac = /Mac|iPhone|iPad/.test(navigator.userAgent);
    let paletteOpen = $state(false);
</script>

<Sidebar.Provider>
    <Sidebar.Root collapsible="icon" variant="inset">
        <Sidebar.Header>
            <Sidebar.Menu>
                <Sidebar.MenuItem>
                    <Sidebar.MenuButton size="lg">
                        {#snippet child({ props })}
                            <a href="#overview" {...props}>
                                <div
                                    class="flex aspect-square size-8 items-center justify-center rounded-lg bg-primary font-serif text-lg font-bold text-primary-foreground"
                                >
                                    A
                                </div>
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
                            <DropdownMenu.Label class="text-xs font-normal text-muted-foreground">Theme</DropdownMenu.Label>
                            <DropdownMenu.RadioGroup value={theme.choice} onValueChange={(choice) => chooseTheme(choice as ThemeChoice)}>
                                <DropdownMenu.RadioItem value="system">Match the system</DropdownMenu.RadioItem>
                                <DropdownMenu.RadioItem value="light">Light</DropdownMenu.RadioItem>
                                <DropdownMenu.RadioItem value="dark">Dark</DropdownMenu.RadioItem>
                            </DropdownMenu.RadioGroup>
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
        <header
            class="sticky top-0 z-20 flex h-14 shrink-0 items-center gap-2 border-b bg-background/90 px-4 backdrop-blur md:rounded-t-xl"
        >
            <Sidebar.Trigger class="-ml-1" />
            <Separator orientation="vertical" class="mr-2 data-vertical:h-4 data-vertical:self-auto" />
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
            <Button
                variant="outline"
                size="sm"
                class="ml-auto w-9 justify-start px-0 font-normal text-muted-foreground sm:w-64 sm:px-3"
                aria-label="Search pages, servers and actions"
                onclick={() => (paletteOpen = true)}
            >
                <SearchIcon class="mx-auto sm:mx-0" />
                <span class="hidden truncate sm:inline">Search or jump to</span>
                <Kbd.Group class="ml-auto hidden sm:inline-flex">
                    <Kbd.Root>{onMac ? "⌘" : "Ctrl"}</Kbd.Root>
                    <Kbd.Root>K</Kbd.Root>
                </Kbd.Group>
            </Button>
            <Badge variant="outline" class="hidden gap-1.5 border-waiting/40 bg-waiting/10 text-waiting md:inline-flex">
                <span class="size-1.5 rounded-full bg-waiting"></span>
                Sample data
            </Badge>
        </header>
        <main class="@container/main flex flex-1 flex-col gap-6 p-4 md:p-6">
            {#if !route}
                <Denied title="No such page" detail={`Nothing in the panel lives at #${path}.`} />
            {:else if !allowed}
                <Denied
                    title="Access denied"
                    detail={`${route.title} needs the ${route.permission} permission, which your role does not grant.`}
                />
            {:else if route.page === "denied"}
                <Denied title="Access denied" detail="This page is where a link you may not follow lands." />
            {:else if route.page === "list"}
                <Lists which={route.path} title={route.title} />
            {:else}
                {#await route.page()}
                    <div class="space-y-3" aria-busy="true" aria-label={`Loading ${route.title}`}>
                        <Skeleton class="h-9 w-56" />
                        <Skeleton class="h-4 w-80 max-w-full" />
                        <Skeleton class="h-48 w-full" />
                    </div>
                {:then loaded}
                    <loaded.default />
                {:catch}
                    <div
                        role="alert"
                        class="flex flex-1 flex-col items-center justify-center gap-4 rounded-xl border border-destructive/30 bg-destructive/5 p-12 text-center"
                    >
                        <h1 class="font-serif text-2xl font-semibold">{route.title} did not load</h1>
                        <p class="max-w-md text-sm text-muted-foreground">
                            The panel could not fetch this page's code, usually because a newer build replaced the one this tab started
                            with, or the connection dropped.
                        </p>
                        <Button variant="outline" onclick={() => window.location.reload()}><RotateCcwIcon />Reload the panel</Button>
                    </div>
                {/await}
            {/if}
        </main>
    </Sidebar.Inset>
</Sidebar.Provider>
<CommandPalette bind:open={paletteOpen} pages={reachable} />
<Toaster position="bottom-right" />
