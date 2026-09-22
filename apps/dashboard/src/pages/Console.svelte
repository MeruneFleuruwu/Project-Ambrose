<!-- Project Ambrose by Imjustchico: The remote console: the app a command goes to, the replies so far in a terminal-style card, and a line to type the next command into, answered locally until 17.05 sends it to the server. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import * as Select from "$lib/components/ui/select/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import { Input } from "$lib/components/ui/input/index.js";
    import SendIcon from "@lucide/svelte/icons/send-horizontal";
    import PageHeader from "../components/PageHeader.svelte";
    import { apps } from "../sample";

    let app = $state("gameserver");
    let line = $state("");
    let history = $state<string[]>(["Ambrose> status", "gameserver 573f769, up 6 h 12 min, running, 9 sessions", "Ambrose> help account", "account create <name> <password>", "account set password <name> <password>"]);

    function send(event: SubmitEvent) {
        event.preventDefault();
        if (line.trim() === "") return;
        history = [...history, `Ambrose> ${line}`, "This is a sample console. Commands run for real once 17.05 lands."];
        line = "";
    }
</script>

<PageHeader title="Console" description="Run server commands without opening the machine's terminal.">
    {#snippet actions()}
        <Select.Root type="single" bind:value={app}>
            <Select.Trigger class="w-48">{app}</Select.Trigger>
            <Select.Content>
                {#each apps as entry (entry.name)}<Select.Item value={entry.name} label={entry.name} />{/each}
            </Select.Content>
        </Select.Root>
    {/snippet}
</PageHeader>

<Card.Root class="gap-0 overflow-hidden py-0 shadow-xs">
    <div class="flex items-center gap-1.5 border-b bg-sidebar px-4 py-2.5">
        <span class="size-2.5 rounded-full bg-destructive/70"></span>
        <span class="size-2.5 rounded-full bg-waiting/70"></span>
        <span class="size-2.5 rounded-full bg-healthy/70"></span>
        <span class="ml-2 font-mono text-xs text-muted-foreground">{app}</span>
    </div>
    <div class="h-[50vh] space-y-1 overflow-y-auto bg-sidebar/60 p-4 font-mono text-sm">
        {#each history as entry, index (index)}
            <div class={entry.startsWith("Ambrose>") ? "text-primary" : "text-muted-foreground"}>{entry}</div>
        {/each}
    </div>
    <form class="flex gap-2 border-t p-3" onsubmit={send}>
        <Input bind:value={line} placeholder="Type a command, like status" class="font-mono" aria-label="Command" />
        <Button type="submit"><SendIcon />Send</Button>
    </form>
</Card.Root>
