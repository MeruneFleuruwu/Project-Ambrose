<!-- Project Ambrose by Imjustchico: An area chart drawn as inline SVG from labelled values: a rounded scale with guide lines, a few time labels that thin out on narrow cards, a crosshair and tooltip under the pointer, and the same values in a hidden table for screen readers. -->
<script lang="ts">
    type Point = { label: string; value: number };
    type Props = { label: string; points: Point[]; unit: string; color?: string; class?: string };
    let { label, points, unit, color = "var(--chart-4)", class: extra = "" }: Props = $props();

    const id = $props.id();
    const width = 600;
    const height = 200;

    const step = $derived.by(() => {
        const rough = (Math.max(1, ...points.map((point) => point.value)) * 1.1) / 4;
        const magnitude = 10 ** Math.floor(Math.log10(rough));
        const steps = [1, 1.2, 1.5, 2, 2.5, 3, 4, 5, 6, 8, 10].map((multiple) => multiple * magnitude);
        return steps.find((candidate) => candidate >= rough && (magnitude < 1 || Number.isInteger(candidate))) ?? 10 * magnitude;
    });
    const top = $derived(step * 4);
    const guides = $derived([4, 3, 2, 1, 0].map((share) => share * step));
    const x = (index: number) => (points.length < 2 ? 0 : (index / (points.length - 1)) * width);
    const y = (value: number) => height - (value / top) * height;
    const line = $derived(points.map((point, index) => `${x(index)},${y(point.value)}`).join(" "));
    const ticks = $derived(points.length < 2 ? [] : [0, 0.25, 0.5, 0.75, 1].map((share) => Math.round(share * (points.length - 1))));

    let hovered = $state<number | null>(null);

    function follow(event: PointerEvent) {
        const box = (event.currentTarget as HTMLElement).getBoundingClientRect();
        const share = Math.min(1, Math.max(0, (event.clientX - box.left) / box.width));
        hovered = Math.round(share * (points.length - 1));
    }
</script>

<figure class={`@container/chart flex flex-col gap-2 ${extra}`}>
    <div class="flex min-h-52 flex-1 gap-2">
        <div class="relative w-8 shrink-0 font-mono text-[11px] text-muted-foreground" aria-hidden="true">
            {#each guides as guide, index (index)}
                <span class="absolute right-0 -translate-y-1/2 tabular-nums" style={`top: ${(1 - guide / top) * 100}%`}>{guide}</span>
            {/each}
        </div>
        <div class="relative flex-1" role="presentation" onpointermove={follow} onpointerleave={() => (hovered = null)}>
            <svg
                viewBox={`0 0 ${width} ${height}`}
                preserveAspectRatio="none"
                class="absolute inset-0 size-full overflow-visible"
                aria-hidden="true"
            >
                <defs>
                    <linearGradient id={`${id}-fill`} x1="0" x2="0" y1="0" y2="1">
                        <stop offset="0%" stop-color={color} stop-opacity="0.45" />
                        <stop offset="100%" stop-color={color} stop-opacity="0.02" />
                    </linearGradient>
                </defs>
                {#each guides as guide, index (index)}
                    <line
                        x1="0"
                        x2={width}
                        y1={y(guide)}
                        y2={y(guide)}
                        stroke="var(--border)"
                        stroke-dasharray="4 4"
                        vector-effect="non-scaling-stroke"
                    />
                {/each}
                <polygon points={`0,${height} ${line} ${width},${height}`} fill={`url(#${id}-fill)`} />
                <polyline
                    points={line}
                    fill="none"
                    stroke={color}
                    stroke-width="2"
                    stroke-linejoin="round"
                    vector-effect="non-scaling-stroke"
                />
            </svg>
            {#if hovered !== null}
                {@const point = points[hovered]}
                {@const left = (x(hovered) / width) * 100}
                <div class="pointer-events-none absolute inset-y-0 w-px bg-muted-foreground/40" style={`left: ${left}%`}></div>
                <div
                    class="pointer-events-none absolute size-2.5 -translate-x-1/2 -translate-y-1/2 rounded-full border-2 border-card"
                    style={`left: ${left}%; top: ${(y(point.value) / height) * 100}%; background: ${color}`}
                ></div>
                <div
                    class={`pointer-events-none absolute top-2 z-10 min-w-32 rounded-lg border bg-popover px-3 py-2 text-xs shadow-md ${left > 60 ? "-translate-x-[calc(100%+12px)]" : "translate-x-3"}`}
                    style={`left: ${left}%`}
                >
                    <div class="text-muted-foreground">{point.label}</div>
                    <div class="mt-1 flex items-center gap-2">
                        <span class="size-2 rounded-sm" style={`background: ${color}`}></span>
                        <span class="font-mono font-medium tabular-nums">{point.value}</span>
                        <span class="text-muted-foreground">{unit}</span>
                    </div>
                </div>
            {/if}
        </div>
    </div>
    <div class="relative ml-10 h-4 font-mono text-[11px] text-muted-foreground" aria-hidden="true">
        {#each ticks as tick, index (index)}
            <span
                class={`absolute whitespace-nowrap ${index === 0 ? "" : index === ticks.length - 1 ? "-translate-x-full" : "-translate-x-1/2"} ${index % 2 === 1 ? "hidden @lg/chart:inline" : ""}`}
                style={`left: ${(x(tick) / width) * 100}%`}>{points[tick].label}</span
            >
        {/each}
    </div>
    <table class="sr-only">
        <caption>{label}</caption>
        <tbody>
            {#each points as point, index (index)}
                <tr><td>{point.label}</td><td>{point.value} {unit}</td></tr>
            {/each}
        </tbody>
    </table>
</figure>
