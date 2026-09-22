<!-- Project Ambrose by Imjustchico: The overview: four headline figures across every app, then one card per app with its state, place, uptime, build, sessions, players against the limit, tick times and their chart, badges, problems with the button that fixes each, the age of the sample it shows, and its power buttons. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import { Progress } from "$lib/components/ui/progress/index.js";
    import { Separator } from "$lib/components/ui/separator/index.js";
    import CircleAlertIcon from "@lucide/svelte/icons/circle-alert";
    import FileTextIcon from "@lucide/svelte/icons/file-text";
    import PlayIcon from "@lucide/svelte/icons/play";
    import RotateCcwIcon from "@lucide/svelte/icons/rotate-ccw";
    import SquareIcon from "@lucide/svelte/icons/square";
    import PageHeader from "../components/PageHeader.svelte";
    import Sparkline from "../components/Sparkline.svelte";
    import StatusBadge from "../components/StatusBadge.svelte";
    import { apps } from "../sample";

    const online = apps.filter((app) => app.state !== "unknown").length;
    const players = apps.reduce((total, app) => total + (app.players ?? 0), 0);
    const problems = apps.reduce((total, app) => total + app.problems.length, 0);
    const sessions = apps.reduce((total, app) => total + (app.sessions ?? 0), 0);

    const figures = [
        { label: "Servers online", value: `${online}/${apps.length}`, tone: online === apps.length ? ("healthy" as const) : ("waiting" as const), note: online === apps.length ? "All up" : "One is stopped" },
        { label: "Players online", value: String(players), tone: "healthy" as const, note: "Peak today 31" },
        { label: "Sessions", value: String(sessions), tone: "healthy" as const, note: "Login and game" },
        { label: "Open problems", value: String(problems), tone: problems > 0 ? ("wrong" as const) : ("healthy" as const), note: problems > 0 ? "Needs a look" : "None" },
    ];
</script>

<PageHeader title="Overview" description="Every server on this machine at a glance.">
    {#snippet actions()}
        <Button variant="outline"><RotateCcwIcon />Restart all</Button>
        <Button><PlayIcon />Start all</Button>
    {/snippet}
</PageHeader>

<div class="grid gap-4 sm:grid-cols-2 xl:grid-cols-4">
    {#each figures as figure (figure.label)}
        <Card.Root class="gap-3 bg-gradient-to-t from-primary/5 to-card shadow-xs">
            <Card.Header>
                <Card.Description>{figure.label}</Card.Description>
                <Card.Title class="font-mono text-3xl font-semibold tabular-nums">{figure.value}</Card.Title>
                <Card.Action><StatusBadge tone={figure.tone}>{figure.note}</StatusBadge></Card.Action>
            </Card.Header>
        </Card.Root>
    {/each}
</div>

<div class="grid items-start gap-4 lg:grid-cols-2 2xl:grid-cols-3">
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
            <Card.Content class="space-y-4">
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
                            <dd class="font-medium tabular-nums">{app.tickAverage} ms <span class="text-muted-foreground">/ {app.tickMax} ms worst</span></dd>
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
                    <Button size="sm"><PlayIcon />Start</Button>
                {:else}
                    <Button size="sm" variant="outline"><RotateCcwIcon />Restart</Button>
                    <Button size="sm" variant="outline" class="text-destructive hover:text-destructive"><SquareIcon />Stop</Button>
                {/if}
                <Button size="sm" variant="ghost" href="#logs"><FileTextIcon />Logs</Button>
                <span class={`ml-auto text-xs ${app.stale ? "text-waiting" : "text-muted-foreground"}`}>{app.stale ? `Last seen ${app.age}` : `Updated ${app.age}`}</span>
            </Card.Footer>
        </Card.Root>
    {/each}
</div>
