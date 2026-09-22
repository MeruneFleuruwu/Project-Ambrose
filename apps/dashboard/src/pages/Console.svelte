<!-- Project Ambrose by Imjustchico: The remote console: the app a command goes to, the replies so far, and a line to type the next command into, answered locally until 17.05 sends it to the server. -->
<script lang="ts">
    import { Button, Card, Heading, Mono, Select, TextField } from "@ambrose/ui";
    import { apps } from "../sample";

    let line = $state("");
    let history = $state<string[]>(["Ambrose> status", "gameserver 573f769, up 6 h 12 min, running, 9 sessions", "Ambrose> help account", "account create <name> <password>", "account set password <name> <password>"]);

    function send(event: SubmitEvent) {
        event.preventDefault();
        if (line.trim() === "")
            return;
        history = [...history, `Ambrose> ${line}`, "This is a sample console. Commands run for real once 17.05 lands."];
        line = "";
    }
</script>

<div class="flex flex-col gap-20">
    <Heading level={1}>Console</Heading>
    <Select id="console-app" label="Send commands to" items={apps.map((app) => ({ value: app.name, label: app.name }))} value="gameserver" class="max-w-320" />
    <Card title="Replies">
        <div class="flex max-h-[50vh] flex-col gap-4 overflow-y-auto">
            {#each history as entry, index (index)}
                <Mono size="13" tone={entry.startsWith("Ambrose>") ? "body" : "muted"}>{entry}</Mono>
            {/each}
        </div>
    </Card>
    <form class="flex flex-wrap items-end gap-8" onsubmit={send}>
        <TextField id="console-line" label="Command" mono value={line} placeholder="status" class="min-w-0 flex-1" oninput={(event) => (line = (event.target as HTMLInputElement).value)} />
        <Button type="submit" variant="action" icon="arrow-right">Send</Button>
    </form>
</div>
