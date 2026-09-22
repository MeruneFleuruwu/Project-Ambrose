<!-- Project Ambrose by Imjustchico: The remote console laid out like Pterodactyl's: the app and its power buttons on top, the console with each app's own output and a prompt that recalls earlier commands with the arrow keys, and the app's address, state, uptime, sessions, tick and build beside it, answered locally until 17.05 sends commands to the server. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import * as Select from "$lib/components/ui/select/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import ActivityIcon from "@lucide/svelte/icons/activity";
    import ClockIcon from "@lucide/svelte/icons/clock";
    import EraserIcon from "@lucide/svelte/icons/eraser";
    import GitCommitIcon from "@lucide/svelte/icons/git-commit-horizontal";
    import NetworkIcon from "@lucide/svelte/icons/network";
    import PlayIcon from "@lucide/svelte/icons/play";
    import RotateCcwIcon from "@lucide/svelte/icons/rotate-ccw";
    import SendIcon from "@lucide/svelte/icons/send-horizontal";
    import SquareIcon from "@lucide/svelte/icons/square";
    import UsersIcon from "@lucide/svelte/icons/users";
    import { toast } from "svelte-sonner";
    import PageHeader from "../components/PageHeader.svelte";
    import StatusBadge from "../components/StatusBadge.svelte";
    import { focus } from "../focus.svelte";
    import { requestPower } from "../power";
    import { apps } from "../sample";

    type Line = { kind: "command" | "reply" | "status"; text: string };

    const app = $derived(apps.find((entry) => entry.name === focus.app) ?? apps[0]);
    const outputs = $state<Record<string, Line[]>>({
        gameserver: [
            { kind: "status", text: "Connected to gameserver" },
            { kind: "command", text: "status" },
            { kind: "reply", text: "gameserver 573f769, up 6 h 12 min, running, 9 sessions" },
            { kind: "command", text: "help account" },
            { kind: "reply", text: "account create <name> <password>\naccount set password <name> <password>" },
        ],
    });
    const output = $derived(outputs[app.name] ?? [{ kind: "status" as const, text: app.state === "unknown" ? `${app.name} is not running` : `Connected to ${app.name}` }]);

    let line = $state("");
    let recall = -1;
    let screen = $state<HTMLDivElement | null>(null);

    const stats = $derived([
        { icon: NetworkIcon, title: "Address", value: `${app.address}:${app.port}`, copy: true, tone: "" },
        { icon: ClockIcon, title: "Uptime", value: app.uptime || "Not running", copy: false, tone: "" },
        { icon: UsersIcon, title: "Sessions", value: app.sessions === null ? "None" : String(app.sessions), copy: false, tone: "" },
        {
            icon: ActivityIcon,
            title: "Tick, average and worst",
            value: app.tickAverage === null ? "No game loop" : `${app.tickAverage} ms / ${app.tickMax} ms`,
            copy: false,
            tone: (app.tickMax ?? 0) > 10 ? "bg-waiting/15 text-waiting" : "",
        },
        { icon: GitCommitIcon, title: "Build", value: app.revision, copy: true, tone: "" },
    ]);

    function history(name: string): string[] {
        try {
            const saved = JSON.parse(window.localStorage.getItem(`ambrose.panel.console.${name}`) ?? "[]");
            return Array.isArray(saved) ? saved.filter((entry) => typeof entry === "string") : [];
        } catch {
            return [];
        }
    }

    function remember(name: string, command: string) {
        const next = [command, ...history(name).filter((entry) => entry !== command)].slice(0, 50);
        try {
            window.localStorage.setItem(`ambrose.panel.console.${name}`, JSON.stringify(next));
        } catch {
            return;
        }
    }

    function answer(command: string): string {
        if (command === "status") return `${app.name} ${app.revision}, ${app.uptime ? `up ${app.uptime}, running` : "stopped"}, ${app.sessions ?? 0} sessions`;
        if (command === "help") return "status, help, account, server, reload\nThis is a sample console: commands run for real once 17.05 lands.";
        return "This is a sample console: commands run for real once 17.05 lands.";
    }

    function send(event: SubmitEvent) {
        event.preventDefault();
        const command = line.trim();
        if (command === "") return;
        outputs[app.name] = [...output, { kind: "command", text: command }, { kind: "reply", text: answer(command) }];
        remember(app.name, command);
        line = "";
        recall = -1;
    }

    function browse(event: KeyboardEvent) {
        if (event.key !== "ArrowUp" && event.key !== "ArrowDown") return;
        const past = history(app.name);
        if (past.length === 0) return;
        event.preventDefault();
        recall = event.key === "ArrowUp" ? Math.min(recall + 1, past.length - 1) : Math.max(recall - 1, -1);
        line = recall === -1 ? "" : past[recall];
    }

    async function copy(text: string) {
        try {
            await navigator.clipboard.writeText(text);
            toast.success(`Copied ${text}`);
        } catch {
            toast.error("The browser did not allow copying");
        }
    }

    $effect(() => {
        if (screen && output.length > 0) screen.scrollTop = screen.scrollHeight;
    });
</script>

<PageHeader title="Console" description="Run server commands without opening the machine's terminal.">
    {#snippet actions()}
        <Select.Root type="single" bind:value={focus.app} onValueChange={() => (recall = -1)}>
            <Select.Trigger class="w-44" aria-label="App">{focus.app}</Select.Trigger>
            <Select.Content>
                {#each apps as entry (entry.name)}<Select.Item value={entry.name} label={entry.name} />{/each}
            </Select.Content>
        </Select.Root>
        {#if app.state === "unknown"}
            <Button onclick={() => requestPower("start", app.name)}><PlayIcon />Start</Button>
        {:else}
            <Button variant="outline" onclick={() => requestPower("restart", app.name)}><RotateCcwIcon />Restart</Button>
            <Button variant="outline" class="text-destructive hover:text-destructive" onclick={() => requestPower("stop", app.name)}><SquareIcon />Stop</Button>
        {/if}
    {/snippet}
</PageHeader>

<div class="grid gap-4 @4xl/main:grid-cols-4">
    <Card.Root class="gap-0 overflow-hidden py-0 shadow-xs @4xl/main:col-span-3">
        <div class="flex items-center gap-3 border-b bg-sidebar px-4 py-2">
            <span class="font-mono text-xs text-muted-foreground">{app.name}</span>
            <StatusBadge tone={app.state} pulse={app.state === "healthy"}>{app.word}</StatusBadge>
            <Button variant="ghost" size="sm" class="ml-auto h-7 text-muted-foreground" onclick={() => (outputs[app.name] = [])}><EraserIcon />Clear</Button>
        </div>
        <div bind:this={screen} class="h-[52vh] overflow-y-auto bg-sidebar/60 p-4 font-mono text-sm leading-6" role="log" aria-label={`Output from ${app.name}`}>
            {#each output as entry, index (index)}
                {#if entry.kind === "command"}
                    <div class="flex gap-2 text-foreground"><span class="text-primary select-none">›</span><span class="break-all">{entry.text}</span></div>
                {:else if entry.kind === "status"}
                    <div class="text-xs text-muted-foreground italic">{entry.text}</div>
                {:else}
                    <div class="pl-4 break-words whitespace-pre-wrap text-muted-foreground">{entry.text}</div>
                {/if}
            {:else}
                <p class="font-sans text-sm text-muted-foreground">Nothing here yet. Type a command below, like status.</p>
            {/each}
        </div>
        <form class="flex items-center gap-2 border-t bg-sidebar/60 px-4 py-2" onsubmit={send}>
            <span class="font-mono text-primary select-none" aria-hidden="true">›</span>
            <input
                bind:value={line}
                onkeydown={browse}
                placeholder={app.state === "unknown" ? `${app.name} is not running` : "Type a command, like status. Up and down recall earlier ones."}
                disabled={app.state === "unknown"}
                class="h-9 min-w-0 flex-1 bg-transparent font-mono text-sm outline-none placeholder:text-muted-foreground disabled:cursor-not-allowed"
                aria-label="Command"
                autocomplete="off"
                spellcheck="false"
            />
            <Button type="submit" size="sm" disabled={app.state === "unknown" || line.trim() === ""}><SendIcon />Send</Button>
        </form>
    </Card.Root>

    <div class="grid content-start gap-3 @md/main:grid-cols-2 @4xl/main:grid-cols-1">
        {#each stats as stat (stat.title)}
            <Card.Root class="gap-0 py-0 shadow-xs">
                <div class="flex items-center gap-3 p-3">
                    <div class={`flex size-10 shrink-0 items-center justify-center rounded-lg ${stat.tone || "bg-muted text-muted-foreground"}`}>
                        <stat.icon class="size-5" />
                    </div>
                    <div class="min-w-0 flex-1">
                        <div class="text-xs text-muted-foreground">{stat.title}</div>
                        {#if stat.copy}
                            <button type="button" class="truncate font-mono text-sm font-medium hover:underline" title="Copy" onclick={() => copy(stat.value)}>{stat.value}</button>
                        {:else}
                            <div class="truncate text-sm font-medium tabular-nums">{stat.value}</div>
                        {/if}
                    </div>
                </div>
            </Card.Root>
        {/each}
    </div>
</div>
