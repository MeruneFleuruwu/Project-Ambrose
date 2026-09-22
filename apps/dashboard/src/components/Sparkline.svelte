<!-- Project Ambrose by Imjustchico: A small area chart drawn as inline SVG from a list of values, with the same values in a hidden table for screen readers. -->
<script lang="ts">
    type Props = { label: string; values: number[]; unit: string };
    let { label, values, unit }: Props = $props();

    const id = $props.id();

    const width = 300;
    const height = 56;
    const top = $derived(Math.max(...values) * 1.1);
    const points = $derived(
        values.map((value, index) => `${(index / (values.length - 1)) * width},${height - (value / top) * height}`).join(" "),
    );
</script>

<figure class="space-y-1">
    <svg viewBox={`0 0 ${width} ${height}`} preserveAspectRatio="none" class="h-14 w-full" aria-hidden="true">
        <defs>
            <linearGradient id={`${id}-fill`} x1="0" x2="0" y1="0" y2="1">
                <stop offset="0%" stop-color="var(--chart-2)" stop-opacity="0.35" />
                <stop offset="100%" stop-color="var(--chart-2)" stop-opacity="0" />
            </linearGradient>
        </defs>
        <polygon points={`0,${height} ${points} ${width},${height}`} fill={`url(#${id}-fill)`} />
        <polyline
            {points}
            fill="none"
            stroke="var(--chart-2)"
            stroke-width="2"
            vector-effect="non-scaling-stroke"
            stroke-linejoin="round"
        />
    </svg>
    <table class="sr-only">
        <caption>{label}</caption>
        <tbody>
            {#each values as value, index (index)}
                <tr><td>{index * 5} s</td><td>{value} {unit}</td></tr>
            {/each}
        </tbody>
    </table>
    <figcaption class="text-xs text-muted-foreground">{label}</figcaption>
</figure>
