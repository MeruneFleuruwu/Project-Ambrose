<!-- Project Ambrose by Imjustchico: The log viewer: an app to follow, a search that marks what it finds, level filters with their counts, a volume chart of the records by level over time, and the records themselves in a monospaced list that follows new lines live, pauses on request, offers a jump back to the newest, wraps on request and opens any record to show and copy its fields. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import * as Select from "$lib/components/ui/select/index.js";
    import * as ToggleGroup from "$lib/components/ui/toggle-group/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import { Input } from "$lib/components/ui/input/index.js";
    import { Label } from "$lib/components/ui/label/index.js";
    import { Switch } from "$lib/components/ui/switch/index.js";
    import ArrowDownIcon from "@lucide/svelte/icons/arrow-down";
    import CopyIcon from "@lucide/svelte/icons/copy";
    import PauseIcon from "@lucide/svelte/icons/pause";
    import PlayIcon from "@lucide/svelte/icons/play";
    import SearchIcon from "@lucide/svelte/icons/search";
    import { tick } from "svelte";
    import { toast } from "svelte-sonner";
    import PageHeader from "../components/PageHeader.svelte";
    import StatusBadge from "../components/StatusBadge.svelte";
    import { focus } from "../focus.svelte";
    import { apps, logRecord, logRecords, type LogRecord } from "../sample";

    const order = ["trace", "debug", "info", "warn", "error", "fatal"] as const;
    const looks: Record<string, { text: string; fill: string }> = {
        trace: { text: "text-muted-foreground", fill: "bg-unknown/50" },
        debug: { text: "text-muted-foreground", fill: "bg-unknown" },
        info: { text: "text-healthy", fill: "bg-healthy" },
        warn: { text: "text-waiting", fill: "bg-waiting" },
        error: { text: "text-destructive", fill: "bg-destructive" },
        fatal: { text: "text-destructive font-bold", fill: "bg-destructive" },
    };

    let records = $state.raw<LogRecord[]>(logRecords);
    let levels = $state<string[]>(["debug", "info", "warn", "error", "fatal"]);
    let search = $state("");
    let live = $state(true);
    let wrap = $state(false);
    let opened = $state<number | null>(null);
    let unseen = $state(0);
    let atBottom = true;
    let viewport = $state<HTMLDivElement | null>(null);

    const needle = $derived(search.trim().toLowerCase());
    const passes = (record: LogRecord) => levels.includes(record.level) && (needle === "" || `${record.category} ${record.message}`.toLowerCase().includes(needle));
    const shown = $derived(records.filter(passes));
    const counts = $derived(Object.fromEntries(order.map((level) => [level, records.filter((record) => record.level === level).length])));

    const buckets = $derived.by(() => {
        const first = records[0]?.seconds ?? 0;
        const span = Math.max(1, (records.at(-1)?.seconds ?? 0) - first + 1);
        const slots = Array.from({ length: 36 }, (_, index) => ({ from: first + (index * span) / 36, tally: {} as Record<string, number>, total: 0 }));
        for (const record of shown) {
            const slot = slots[Math.min(35, Math.floor(((record.seconds - first) / span) * 36))];
            slot.tally[record.level] = (slot.tally[record.level] ?? 0) + 1;
            slot.total += 1;
        }
        return slots;
    });
    const busiest = $derived(Math.max(1, ...buckets.map((slot) => slot.total)));

    function clock(seconds: number) {
        const whole = Math.floor(seconds);
        return [Math.floor(whole / 3600), Math.floor(whole / 60) % 60, whole % 60].map((part) => String(part).padStart(2, "0")).join(":");
    }

    function pieces(message: string) {
        if (needle === "") return [{ text: message, hit: false }];
        const parts: { text: string; hit: boolean }[] = [];
        const lower = message.toLowerCase();
        let from = 0;
        for (let at = lower.indexOf(needle); at !== -1; at = lower.indexOf(needle, from)) {
            if (at > from) parts.push({ text: message.slice(from, at), hit: false });
            parts.push({ text: message.slice(at, at + needle.length), hit: true });
            from = at + needle.length;
        }
        if (from < message.length) parts.push({ text: message.slice(from), hit: false });
        return parts;
    }

    async function toNewest() {
        await tick();
        if (viewport) viewport.scrollTop = viewport.scrollHeight;
        atBottom = true;
        unseen = 0;
    }

    function followScroll() {
        if (!viewport) return;
        atBottom = viewport.scrollHeight - viewport.scrollTop - viewport.clientHeight < 24;
        if (atBottom) unseen = 0;
    }

    async function toggle(sequence: number) {
        opened = opened === sequence ? null : sequence;
        await tick();
        if (opened !== null) document.getElementById(`record-${opened}`)?.scrollIntoView({ block: "nearest" });
    }

    async function copy(record: LogRecord) {
        try {
            await navigator.clipboard.writeText(JSON.stringify({ app: focus.app, ...record }));
            toast.success(`Copied record ${record.sequence}`);
        } catch {
            toast.error("The browser did not allow copying");
        }
    }

    $effect(() => {
        if (viewport) void toNewest();
    });

    $effect(() => {
        if (!live) return;
        const timer = setInterval(() => {
            const next = logRecord((records.at(-1)?.sequence ?? 999) - 999);
            records = [...records.slice(-499), next];
            if (!passes(next)) return;
            if (atBottom) void toNewest();
            else unseen += 1;
        }, 1500);
        return () => clearInterval(timer);
    });
</script>

<PageHeader title="Logs" description="Follow a server's log as it is written.">
    {#snippet actions()}
        {#if live}
            <StatusBadge tone="healthy" pulse>Following live</StatusBadge>
            <Button variant="outline" size="sm" onclick={() => (live = false)}><PauseIcon />Pause</Button>
        {:else}
            <StatusBadge tone="waiting">Paused</StatusBadge>
            <Button variant="outline" size="sm" onclick={() => (live = true)}><PlayIcon />Resume</Button>
        {/if}
    {/snippet}
</PageHeader>

<div class="flex flex-col gap-3 @4xl/main:flex-row @4xl/main:items-center">
    <div class="flex flex-1 gap-3">
        <Select.Root type="single" bind:value={focus.app}>
            <Select.Trigger class="w-40 shrink-0" aria-label="App">{focus.app}</Select.Trigger>
            <Select.Content>
                {#each apps as entry (entry.name)}<Select.Item value={entry.name} label={entry.name} />{/each}
            </Select.Content>
        </Select.Root>
        <div class="relative flex-1">
            <SearchIcon class="absolute top-1/2 left-2.5 size-4 -translate-y-1/2 text-muted-foreground" />
            <Input type="search" placeholder="Search logs" class="pl-8" aria-label="Search messages and categories" bind:value={search} />
        </div>
    </div>
    <div class="flex flex-wrap items-center gap-3">
        <ToggleGroup.Root type="multiple" variant="outline" size="sm" spacing={1} class="flex-wrap" bind:value={levels} aria-label="Levels to show">
            {#each order as level (level)}
                <ToggleGroup.Item value={level} class="gap-1.5 px-2.5">
                    <span class={`size-1.5 rounded-full ${looks[level].fill}`}></span>{level}
                    <span class="font-mono text-[11px] tabular-nums opacity-70">{counts[level]}</span>
                </ToggleGroup.Item>
            {/each}
        </ToggleGroup.Root>
        <div class="flex items-center gap-2">
            <Switch id="wrap-lines" bind:checked={wrap} />
            <Label for="wrap-lines" class="text-sm font-normal whitespace-nowrap">Wrap lines</Label>
        </div>
    </div>
</div>

<Card.Root class="gap-0 overflow-hidden py-0 shadow-xs">
    <div class="border-b px-4 pt-3 pb-2">
        <div class="flex h-14 items-end gap-px" aria-hidden="true">
            {#each buckets as slot, index (index)}
                <div
                    class="flex h-full flex-1 flex-col-reverse"
                    title={`${clock(slot.from)}: ${slot.total} record${slot.total === 1 ? "" : "s"}${order
                        .filter((level) => slot.tally[level])
                        .map((level) => `, ${slot.tally[level]} ${level}`)
                        .join("")}`}
                >
                    {#each order as level (level)}
                        {#if slot.tally[level]}
                            <div class={`${looks[level].fill} first:rounded-b-[1px] last:rounded-t-[1px]`} style={`height: ${(slot.tally[level] / busiest) * 100}%`}></div>
                        {/if}
                    {/each}
                </div>
            {/each}
        </div>
        <div class="mt-1.5 flex justify-between font-mono text-[11px] text-muted-foreground">
            <span>{clock(buckets[0].from)}</span>
            <span class="sr-only">Records by level over time, {shown.length} records in all</span>
            <span>{clock(records.at(-1)?.seconds ?? 0)}</span>
        </div>
    </div>
    <div class="relative">
        <div bind:this={viewport} onscroll={followScroll} class="h-[58vh] overflow-y-auto bg-sidebar/60 py-2 font-mono text-xs leading-6" role="log" aria-live="off">
            {#each shown as record (record.sequence)}
                <div class={opened === record.sequence ? "bg-muted/60" : ""}>
                    <button
                        type="button"
                        class="flex w-full items-stretch gap-3 px-3 text-left hover:bg-muted/60 focus-visible:bg-muted/60 focus-visible:outline-none"
                        aria-expanded={opened === record.sequence}
                        onclick={() => toggle(record.sequence)}
                    >
                        <span class={`my-1 w-0.5 shrink-0 rounded-full ${looks[record.level].fill}`}></span>
                        <span class="hidden shrink-0 text-muted-foreground tabular-nums sm:inline">{record.time.slice(11)}</span>
                        <span class={`w-11 shrink-0 uppercase ${looks[record.level].text}`}>{record.level}</span>
                        <span class="hidden w-32 shrink-0 truncate text-chart-5 md:inline">{record.category}</span>
                        <span class={`min-w-0 flex-1 ${wrap ? "break-words whitespace-pre-wrap" : "truncate"}`}
                            >{#each pieces(record.message) as piece, index (index)}{#if piece.hit}<mark class="rounded-sm bg-primary/35 text-foreground">{piece.text}</mark
                                    >{:else}{piece.text}{/if}{/each}</span
                        >
                    </button>
                    {#if opened === record.sequence}
                        <div id={`record-${record.sequence}`} class="mx-3 mb-2 ml-7 rounded-md border bg-card p-3 font-sans">
                            <dl class="grid grid-cols-[auto_1fr] gap-x-6 gap-y-1 text-xs">
                                <dt class="text-muted-foreground">Sequence</dt>
                                <dd class="font-mono">{record.sequence}</dd>
                                <dt class="text-muted-foreground">Time</dt>
                                <dd class="font-mono">{record.time}</dd>
                                <dt class="text-muted-foreground">App</dt>
                                <dd class="font-mono">{focus.app}</dd>
                                <dt class="text-muted-foreground">Level</dt>
                                <dd class={`font-mono ${looks[record.level].text}`}>{record.level}</dd>
                                <dt class="text-muted-foreground">Category</dt>
                                <dd class="font-mono">{record.category}</dd>
                                <dt class="text-muted-foreground">Message</dt>
                                <dd class="font-mono break-words">{record.message}</dd>
                            </dl>
                            <Button size="sm" variant="outline" class="mt-3" onclick={() => copy(record)}><CopyIcon />Copy as JSON</Button>
                        </div>
                    {/if}
                </div>
            {:else}
                <p class="py-12 text-center font-sans text-sm text-muted-foreground">No record matches these levels and this search.</p>
            {/each}
        </div>
        {#if unseen > 0}
            <Button size="sm" class="absolute bottom-3 left-1/2 -translate-x-1/2 shadow-md" onclick={toNewest}>
                <ArrowDownIcon />{unseen} new record{unseen === 1 ? "" : "s"}
            </Button>
        {/if}
    </div>
    <div class="flex items-center justify-between border-t px-4 py-2 text-xs text-muted-foreground">
        <span>Showing {shown.length} of {records.length} records, newest at the bottom</span>
        <span class="hidden sm:inline">Sample data for {focus.app}</span>
    </div>
</Card.Root>
