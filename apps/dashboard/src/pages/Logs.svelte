<!-- Project Ambrose by Imjustchico: The log viewer: an app and a lowest level to follow, a search box, and the records that pass them in a scrolling monospaced list with each level coloured. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import * as Select from "$lib/components/ui/select/index.js";
    import { Input } from "$lib/components/ui/input/index.js";
    import SearchIcon from "@lucide/svelte/icons/search";
    import PageHeader from "../components/PageHeader.svelte";
    import StatusBadge from "../components/StatusBadge.svelte";
    import { apps, logRecords } from "../sample";

    const order = ["trace", "debug", "info", "warn", "error", "fatal"];
    let app = $state("gameserver");
    let level = $state("info");
    let search = $state("");

    const shown = $derived(logRecords.filter((record) => order.indexOf(record.level) >= order.indexOf(level) && (search === "" || record.message.toLowerCase().includes(search.toLowerCase()))));
    const colours: Record<string, string> = {
        trace: "text-muted-foreground",
        debug: "text-muted-foreground",
        info: "text-healthy",
        warn: "text-waiting",
        error: "text-destructive",
        fatal: "text-destructive font-bold",
    };
</script>

<PageHeader title="Logs" description="Follow a server's log as it is written.">
    {#snippet actions()}
        <StatusBadge tone="healthy" pulse>Following live</StatusBadge>
    {/snippet}
</PageHeader>

<div class="flex flex-col gap-3 sm:flex-row">
    <Select.Root type="single" bind:value={app}>
        <Select.Trigger class="w-full sm:w-48">{app}</Select.Trigger>
        <Select.Content>
            {#each apps as entry (entry.name)}<Select.Item value={entry.name} label={entry.name} />{/each}
        </Select.Content>
    </Select.Root>
    <Select.Root type="single" bind:value={level}>
        <Select.Trigger class="w-full sm:w-40">From {level}</Select.Trigger>
        <Select.Content>
            {#each order as name (name)}<Select.Item value={name} label={name} />{/each}
        </Select.Content>
    </Select.Root>
    <div class="relative flex-1">
        <SearchIcon class="absolute top-1/2 left-2.5 size-4 -translate-y-1/2 text-muted-foreground" />
        <Input type="search" placeholder="Search messages" class="pl-8" aria-label="Search messages" bind:value={search} />
    </div>
</div>

<Card.Root class="gap-0 overflow-hidden py-0 shadow-xs">
    <div class="h-[60vh] overflow-y-auto bg-sidebar/60 p-4 font-mono text-xs leading-6">
        {#each shown as record (record.sequence)}
            <div class="flex gap-3 rounded px-1 hover:bg-muted/60">
                <span class="hidden shrink-0 text-muted-foreground sm:inline">{record.time}</span>
                <span class={`w-12 shrink-0 uppercase ${colours[record.level]}`}>{record.level}</span>
                <span class="hidden w-36 shrink-0 truncate text-chart-3 md:inline">{record.category}</span>
                <span class="min-w-0 break-words">{record.message}</span>
            </div>
        {:else}
            <p class="py-12 text-center font-sans text-sm text-muted-foreground">No record matches the level and search.</p>
        {/each}
    </div>
</Card.Root>
