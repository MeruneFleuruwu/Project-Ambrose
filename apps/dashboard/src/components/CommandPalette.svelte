<!-- Project Ambrose by Imjustchico: The command palette that Ctrl+K or Cmd+K opens from anywhere: type to jump to a page the viewer may use, open an app's console or logs, send a power action, or switch between light and dark. -->
<script lang="ts">
    import * as Command from "$lib/components/ui/command/index.js";
    import { chooseTheme, type ThemeChoice } from "$lib/theme.svelte.js";
    import FileTextIcon from "@lucide/svelte/icons/file-text";
    import MonitorIcon from "@lucide/svelte/icons/monitor";
    import MoonIcon from "@lucide/svelte/icons/moon";
    import PlayIcon from "@lucide/svelte/icons/play";
    import RotateCcwIcon from "@lucide/svelte/icons/rotate-ccw";
    import SquareTerminalIcon from "@lucide/svelte/icons/square-terminal";
    import SunIcon from "@lucide/svelte/icons/sun";
    import { openFor } from "../focus.svelte";
    import { requestPower } from "../power";
    import type { Route } from "../routes";
    import { apps } from "../sample";

    type Props = { open: boolean; pages: Route[] };
    let { open = $bindable(false), pages }: Props = $props();

    const looks: { choice: ThemeChoice; label: string; icon: typeof SunIcon }[] = [
        { choice: "system", label: "Match the system theme", icon: MonitorIcon },
        { choice: "light", label: "Light theme", icon: SunIcon },
        { choice: "dark", label: "Dark theme", icon: MoonIcon },
    ];

    $effect(() => {
        const toggle = (event: KeyboardEvent) => {
            if (event.key.toLowerCase() === "k" && (event.metaKey || event.ctrlKey)) {
                event.preventDefault();
                open = !open;
            }
        };
        document.addEventListener("keydown", toggle);
        return () => document.removeEventListener("keydown", toggle);
    });

    function run(action: () => void) {
        open = false;
        action();
    }
</script>

<Command.Dialog bind:open title="Command palette" description="Jump to a page, open a server's console or logs, or run an action.">
    <Command.Input placeholder="Type a page, a server or an action" />
    <Command.List>
        <Command.Empty>Nothing matches that.</Command.Empty>
        <Command.Group heading="Pages">
            {#each pages as page (page.path)}
                <Command.Item value={`page ${page.title}`} onSelect={() => run(() => (window.location.hash = `#${page.path}`))}>
                    <page.icon />
                    <span>{page.title}</span>
                </Command.Item>
            {/each}
        </Command.Group>
        <Command.Separator />
        <Command.Group heading="Servers">
            {#each apps as app (app.name)}
                <Command.Item value={`${app.name} console`} keywords={[app.role, "terminal", "command"]} onSelect={() => run(() => openFor(app.name, "console"))}>
                    <SquareTerminalIcon />
                    <span>Open the {app.name} console</span>
                </Command.Item>
                <Command.Item value={`${app.name} logs`} keywords={[app.role, "log", "records"]} onSelect={() => run(() => openFor(app.name, "logs"))}>
                    <FileTextIcon />
                    <span>Follow the {app.name} logs</span>
                </Command.Item>
                {#if app.state === "unknown"}
                    <Command.Item value={`start ${app.name}`} onSelect={() => run(() => requestPower("start", app.name))}>
                        <PlayIcon />
                        <span>Start {app.name}</span>
                    </Command.Item>
                {:else}
                    <Command.Item value={`restart ${app.name}`} onSelect={() => run(() => requestPower("restart", app.name))}>
                        <RotateCcwIcon />
                        <span>Restart {app.name}</span>
                    </Command.Item>
                {/if}
            {/each}
        </Command.Group>
        <Command.Separator />
        <Command.Group heading="Appearance">
            {#each looks as look (look.choice)}
                <Command.Item value={`theme ${look.label}`} keywords={["theme", "mode", "appearance"]} onSelect={() => run(() => chooseTheme(look.choice))}>
                    <look.icon />
                    <span>{look.label}</span>
                </Command.Item>
            {/each}
        </Command.Group>
    </Command.List>
</Command.Dialog>
