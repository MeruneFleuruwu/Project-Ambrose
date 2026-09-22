<!-- Project Ambrose by Imjustchico: The overview: four headline figures with a line on what each means, one card per app with its state, place, uptime, build, sessions, players against the limit, tick times and their chart, badges, problems with the button that fixes each, the age of the sample and its power buttons, then players online over a chosen span beside the panel's latest activity. -->
<script lang="ts">
    import * as Avatar from "$lib/components/ui/avatar/index.js";
    import * as Card from "$lib/components/ui/card/index.js";
    import * as Select from "$lib/components/ui/select/index.js";
    import * as ToggleGroup from "$lib/components/ui/toggle-group/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import { Progress } from "$lib/components/ui/progress/index.js";
    import { Separator } from "$lib/components/ui/separator/index.js";
    import ArrowRightIcon from "@lucide/svelte/icons/arrow-right";
    import CircleAlertIcon from "@lucide/svelte/icons/circle-alert";
    import FileTextIcon from "@lucide/svelte/icons/file-text";
    import PlayIcon from "@lucide/svelte/icons/play";
    import RotateCcwIcon from "@lucide/svelte/icons/rotate-ccw";
    import SquareIcon from "@lucide/svelte/icons/square";
    import AreaChart from "../components/AreaChart.svelte";
    import PageHeader from "../components/PageHeader.svelte";
    import Sparkline from "../components/Sparkline.svelte";
    import StatusBadge from "../components/StatusBadge.svelte";
    import { openFor } from "../focus.svelte";
    import { requestPower } from "../power";
    import { activity, apps, playerHistory } from "../sample";

    const online = apps.filter((app) => app.state !== "unknown");
    const stopped = apps.filter((app) => app.state === "unknown");
    const players = apps.reduce((total, app) => total + (app.players ?? 0), 0);
    const sessions = apps.reduce((total, app) => total + (app.sessions ?? 0), 0);
    const problems = apps.flatMap((app) => app.problems);
    const peak = playerHistory.day.reduce((best, point) => (point.value > best.value ? point : best));

    const figures = [
        {
            label: "Servers online",
            value: `${online.length}/${apps.length}`,
            tone: stopped.length === 0 ? ("healthy" as const) : ("waiting" as const),
            badge: stopped.length === 0 ? "All up" : `${stopped.length} stopped`,
            headline: stopped.length === 0 ? "Every app is answering" : `${stopped.map((app) => app.name).join(", ")} is not running`,
            detail: "Checked every 5 seconds",
        },
        {
            label: "Players online",
            value: String(players),
            tone: "healthy" as const,
            badge: "+2 this hour",
            headline: `Peak ${peak.value} at ${peak.label} in the last day`,
            detail: "On one realm with room for 500",
        },
        {
            label: "Sessions",
            value: String(sessions),
            tone: "healthy" as const,
            badge: "All responding",
            headline: apps
                .filter((app) => app.sessions !== null)
                .map((app) => `${app.sessions} on ${app.role}`)
                .join(", "),
            detail: "Each connected client holds one",
        },
        {
            label: "Open problems",
            value: String(problems.length),
            tone: problems.length > 0 ? ("wrong" as const) : ("healthy" as const),
            badge: problems.length > 0 ? "Needs a look" : "None",
            headline: problems.length > 0 ? problems[0].message : "Nothing needs fixing",
            detail:
                problems.length > 0 ? `The fix is on ${problems[0].fixLabel.replace(/^Open /, "")}` : "Problems show here as they appear",
        },
    ];

    const spans = [
        { value: "hour", label: "Last hour" },
        { value: "day", label: "Last 24 hours" },
        { value: "week", label: "Last 7 days" },
    ] as const;
    let span = $state<"hour" | "day" | "week">("day");
    const spanLabel = $derived(spans.find((entry) => entry.value === span)?.label ?? "");

    const initials = (name: string) => name.slice(0, 2).toUpperCase();
</script>

<PageHeader title="Overview" description="Every server on this machine at a glance.">
    {#snippet actions()}
        <Button variant="outline" onclick={() => requestPower("restart", "every app")}><RotateCcwIcon />Restart all</Button>
        <Button onclick={() => requestPower("start", "every app")}><PlayIcon />Start all</Button>
    {/snippet}
</PageHeader>

<div class="grid gap-4 @xl/main:grid-cols-2 @5xl/main:grid-cols-4">
    {#each figures as figure (figure.label)}
        <Card.Root class="@container/card gap-4 bg-gradient-to-t from-primary/5 to-card shadow-xs">
            <Card.Header>
                <Card.Description>{figure.label}</Card.Description>
                <Card.Title class="font-mono text-2xl font-semibold tabular-nums @[250px]/card:text-3xl">{figure.value}</Card.Title>
                <Card.Action><StatusBadge tone={figure.tone}>{figure.badge}</StatusBadge></Card.Action>
            </Card.Header>
            <Card.Footer class="flex-col items-start gap-1 text-sm">
                <div class="line-clamp-1 font-medium">{figure.headline}</div>
                <div class="text-muted-foreground">{figure.detail}</div>
            </Card.Footer>
        </Card.Root>
    {/each}
</div>

<div class="grid gap-4 @2xl/main:grid-cols-2 @5xl/main:grid-cols-3">
    {#each apps as app (app.name)}
        <Card.Root class="shadow-xs">
            <Card.Header>
                <Card.Title class="flex items-center gap-2 font-serif text-xl">
                    {app.name}
                    <span class="rounded-md border px-1.5 py-0.5 font-sans text-xs font-normal text-muted-foreground">{app.role}</span>
                </Card.Title>
                <Card.Description class="font-mono text-xs">{app.address}:{app.port}</Card.Description>
                <Card.Action>
                    <StatusBadge tone={app.state} pulse={app.state === "healthy"}>{app.word}</StatusBadge>
                </Card.Action>
            </Card.Header>
            <Card.Content class="flex-1 space-y-4">
                {#if app.realm || app.badges.length > 0}
                    <div class="flex flex-wrap gap-1.5">
                        {#if app.realm}<StatusBadge tone="mine">{app.realm}</StatusBadge>{/if}
                        {#each app.badges as badge (badge.text)}<StatusBadge tone={badge.tone}>{badge.text}</StatusBadge>{/each}
                    </div>
                {/if}
                <dl class="grid grid-cols-2 gap-x-4 gap-y-3 text-sm">
                    <div>
                        <dt class="text-xs text-muted-foreground">Uptime</dt>
                        <dd class="font-medium">{app.uptime || "Not running"}</dd>
                    </div>
                    <div>
                        <dt class="text-xs text-muted-foreground">Build</dt>
                        <dd class="font-mono font-medium">{app.revision}</dd>
                    </div>
                    <div>
                        <dt class="text-xs text-muted-foreground">Sessions</dt>
                        <dd class="font-medium tabular-nums">{app.sessions ?? "None"}</dd>
                    </div>
                    {#if app.tickAverage !== null}
                        <div>
                            <dt class="text-xs text-muted-foreground">Tick</dt>
                            <dd class="font-medium tabular-nums">
                                {app.tickAverage} ms <span class="text-muted-foreground">/ {app.tickMax} worst</span>
                            </dd>
                        </div>
                    {/if}
                </dl>
                {#if app.players !== null && app.limit !== null}
                    <div class="space-y-1.5">
                        <div class="flex justify-between text-xs">
                            <span class="text-muted-foreground">Players against the realm limit</span>
                            <span class="font-mono tabular-nums">{app.players} / {app.limit}</span>
                        </div>
                        <Progress value={app.players} max={app.limit} class="h-1.5" />
                    </div>
                {/if}
                {#if app.ticks.length > 0}
                    <Sparkline label="Tick time over the last minute" values={app.ticks} unit="ms" />
                {/if}
                {#each app.problems as problem (problem.code)}
                    <div class="flex items-start gap-3 rounded-lg border border-destructive/30 bg-destructive/5 p-3">
                        <CircleAlertIcon class="mt-0.5 size-4 shrink-0 text-destructive" />
                        <div class="flex-1 space-y-2">
                            <p class="text-sm">{problem.message}</p>
                            <Button size="sm" variant="outline" href={`#${problem.fix}`}>{problem.fixLabel}</Button>
                        </div>
                    </div>
                {/each}
            </Card.Content>
            <Separator />
            <Card.Footer class="flex flex-wrap items-center gap-2">
                {#if app.state === "unknown"}
                    <Button size="sm" onclick={() => requestPower("start", app.name)}><PlayIcon />Start</Button>
                {:else}
                    <Button size="sm" variant="outline" onclick={() => requestPower("restart", app.name)}><RotateCcwIcon />Restart</Button>
                    <Button
                        size="sm"
                        variant="outline"
                        class="text-destructive hover:text-destructive"
                        onclick={() => requestPower("stop", app.name)}
                    >
                        <SquareIcon />Stop
                    </Button>
                {/if}
                <Button size="sm" variant="ghost" onclick={() => openFor(app.name, "logs")}><FileTextIcon />Logs</Button>
                <span class={`ml-auto text-xs ${app.stale ? "text-waiting" : "text-muted-foreground"}`}
                    >{app.stale ? `Last seen ${app.age}` : `Updated ${app.age}`}</span
                >
            </Card.Footer>
        </Card.Root>
    {/each}
</div>

<div class="grid gap-4 @5xl/main:grid-cols-3">
    <Card.Root class="@container/card shadow-xs @5xl/main:col-span-2">
        <Card.Header>
            <Card.Title>Players online</Card.Title>
            <Card.Description>{spanLabel}, across every realm</Card.Description>
            <Card.Action>
                <ToggleGroup.Root
                    type="single"
                    variant="outline"
                    size="sm"
                    class="hidden @[560px]/card:flex"
                    value={span}
                    onValueChange={(value) => value && (span = value as typeof span)}
                >
                    {#each spans as entry (entry.value)}<ToggleGroup.Item value={entry.value} class="px-3">{entry.label}</ToggleGroup.Item
                        >{/each}
                </ToggleGroup.Root>
                <Select.Root type="single" value={span} onValueChange={(value) => value && (span = value as typeof span)}>
                    <Select.Trigger size="sm" class="w-36 @[560px]/card:hidden" aria-label="Span of the chart">{spanLabel}</Select.Trigger>
                    <Select.Content>
                        {#each spans as entry (entry.value)}<Select.Item value={entry.value} label={entry.label} />{/each}
                    </Select.Content>
                </Select.Root>
            </Card.Action>
        </Card.Header>
        <Card.Content class="flex flex-1 flex-col">
            <AreaChart label={`Players online, ${spanLabel.toLowerCase()}`} points={playerHistory[span]} unit="players" class="flex-1" />
        </Card.Content>
    </Card.Root>

    <Card.Root class="shadow-xs">
        <Card.Header>
            <Card.Title>Recent activity</Card.Title>
            <Card.Description>What people and schedules did today</Card.Description>
        </Card.Header>
        <Card.Content class="flex-1">
            <ol class="space-y-4">
                {#each activity as entry, index (index)}
                    <li class="flex items-start gap-3">
                        <Avatar.Root class="size-8 rounded-lg">
                            <Avatar.Fallback class="rounded-lg text-xs">{initials(entry.who)}</Avatar.Fallback>
                        </Avatar.Root>
                        <div class="min-w-0 flex-1 text-sm">
                            <p class="leading-snug">
                                <span class="font-medium">{entry.who}</span> <span class="text-muted-foreground">{entry.what}</span>
                            </p>
                            <p class="mt-0.5 text-xs text-muted-foreground">
                                <span class="font-mono tabular-nums">{entry.when}</span> · {entry.where}
                            </p>
                        </div>
                    </li>
                {/each}
            </ol>
        </Card.Content>
        <Card.Footer>
            <Button variant="ghost" size="sm" class="-ml-2" href="#activity">See all activity<ArrowRightIcon /></Button>
        </Card.Footer>
    </Card.Root>
</div>
