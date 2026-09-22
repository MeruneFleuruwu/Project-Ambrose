<!-- Project Ambrose by Imjustchico: The log viewer: an app and a lowest level to follow, a search box, and the records that pass them in the shared log list. -->
<script lang="ts">
    import { Heading, LiveDot, LogList, Select, TextField } from "@ambrose/ui";
    import { apps, logRecords } from "../sample";

    const order = ["trace", "debug", "info", "warn", "error", "fatal"];
    let level = $state("info");
    let search = $state("");

    const shown = $derived(
        logRecords.filter((record) => order.indexOf(record.level) >= order.indexOf(level) && (search === "" || record.message.toLowerCase().includes(search.toLowerCase()))),
    );
</script>

<div class="flex flex-col gap-20">
    <div class="flex flex-wrap items-center justify-between gap-12">
        <Heading level={1}>Logs</Heading>
        <LiveDot age="following live" />
    </div>
    <div class="grid grid-cols-1 gap-12 md:grid-cols-3">
        <Select id="log-app" label="App" items={apps.map((app) => ({ value: app.name, label: app.name }))} value="gameserver" />
        <Select id="log-level" label="Lowest level" items={order.map((name) => ({ value: name, label: name }))} bind:value={level} />
        <TextField id="log-search" label="Search" type="search" placeholder="Words in the message" value={search} oninput={(event) => (search = (event.target as HTMLInputElement).value)} />
    </div>
    <LogList label="gameserver log" records={shown} height="60vh" noResults="No record matches the level and search." status={shown.length === 0 ? "no-results" : "ready"} />
</div>
