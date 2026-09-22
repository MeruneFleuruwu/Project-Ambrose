<!-- Project Ambrose by Imjustchico: The overview: totals across every app, then one card per app with its state, place, uptime, build, sessions, players against the limit, tick times, badges, problems with the button that fixes each, and the age of the sample it shows. -->
<script lang="ts">
    import { Badge, Button, Card, Heading, LiveDot, Mono, ProgressBar, Sparkline, StateDot, StatTile } from "@ambrose/ui";
    import { apps } from "../sample";

    const online = apps.filter((app) => app.state !== "unknown").length;
    const players = apps.reduce((total, app) => total + (app.players ?? 0), 0);
    const problems = apps.reduce((total, app) => total + app.problems.length, 0);
</script>

<div class="flex flex-col gap-20">
    <Heading level={1}>Overview</Heading>
    <div class="grid grid-cols-1 gap-16 md:grid-cols-3">
        <StatTile label="Servers online" value={`${online} of ${apps.length}`} state={online === apps.length ? "healthy" : "waiting"} word={online === apps.length ? "All up" : "Some offline"} />
        <StatTile label="Players online" value={String(players)} state="healthy" word="Across every realm" />
        <StatTile label="Open problems" value={String(problems)} state={problems > 0 ? "wrong" : "healthy"} word={problems > 0 ? "Needs a look" : "None"} />
    </div>
    <div class="grid grid-cols-1 items-start gap-16 lg:grid-cols-2">
        {#each apps as app (app.name)}
            <Card title={app.name}>
                {#snippet actions()}
                    <LiveDot age={app.age} stale={app.stale} />
                {/snippet}
                <div class="flex flex-col gap-12">
                    <div class="flex flex-wrap items-center gap-8">
                        <StateDot state={app.state} word={app.word} />
                        <Badge>{app.role}</Badge>
                        {#if app.realm}<Badge tone="mine">{app.realm}</Badge>{/if}
                        {#each app.badges as badge (badge.text)}<Badge tone={badge.tone}>{badge.text}</Badge>{/each}
                    </div>
                    <dl class="grid grid-cols-2 gap-8 text-13">
                        <dt class="text-fg-muted">Address</dt><dd><Mono size="13">{app.address}:{app.port}</Mono></dd>
                        <dt class="text-fg-muted">Uptime</dt><dd>{app.uptime || "Not running"}</dd>
                        <dt class="text-fg-muted">Build</dt><dd><Mono size="13">{app.revision}</Mono></dd>
                        <dt class="text-fg-muted">Sessions</dt><dd>{app.sessions ?? "None reported"}</dd>
                        {#if app.tickAverage !== null}
                            <dt class="text-fg-muted">Tick</dt><dd>{app.tickAverage} ms average, {app.tickMax} ms worst</dd>
                        {/if}
                    </dl>
                    {#if app.players !== null && app.limit !== null}
                        <ProgressBar label="Players against the realm limit" value={app.players} max={app.limit} detail={`${app.players} of ${app.limit}`} />
                    {/if}
                    {#if app.ticks.length > 0}
                        <Sparkline label="Tick time over the last minute" times={app.ticks.map((_, index) => index * 5)} values={app.ticks} unit="ms" height={40} />
                    {/if}
                    {#each app.problems as problem (problem.code)}
                        <div class="flex flex-wrap items-center justify-between gap-8 rounded border border-edge-quiet p-12">
                            <span class="text-13">{problem.message}</span>
                            <Button variant="quiet" href={`#${problem.fix}`}>{problem.fixLabel}</Button>
                        </div>
                    {/each}
                    <div class="flex flex-wrap gap-8">
                        {#if app.state === "unknown"}
                            <Button variant="action" icon="play">Start</Button>
                        {:else}
                            <Button variant="quiet" icon="rotate-ccw">Restart</Button>
                            <Button variant="danger" icon="square">Stop</Button>
                        {/if}
                        <Button variant="ghost" href="#logs" icon="file-text">Logs</Button>
                    </div>
                </div>
            </Card>
        {/each}
    </div>
</div>
