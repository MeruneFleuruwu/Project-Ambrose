<!-- Project Ambrose by Imjustchico: The page a link lands on when the viewer may not use its route or no route matches, with the way back to the overview. -->
<script lang="ts">
    import { Button, EmptyState } from "@ambrose/ui";

    type Props = { title: string; detail: string };
    let { title, detail }: Props = $props();
</script>

<EmptyState {title} {detail} icon="lock">
    {#snippet action()}
        <Button variant="quiet" href="#overview" icon="arrow-left">Back to the overview</Button>
    {/snippet}
</EmptyState>
