<!-- Project Ambrose by Imjustchico: The client data page: the install the servers read, its revision, the type dump built from it and whether it is current, with the button that rebuilds it. -->
<script lang="ts">
    import { Button, Card, Heading, Mono, StateDot } from "@ambrose/ui";
</script>

<div class="flex flex-col gap-20">
    <Heading level={1}>Client data</Heading>
    <div class="grid grid-cols-1 gap-16 lg:grid-cols-2">
        <Card title="Install">
            <dl class="grid grid-cols-1 gap-8 text-13 sm:grid-cols-[auto_1fr]">
                <dt class="text-fg-muted">Folder</dt><dd><Mono size="13">C:\ProgramData\KingsIsle Entertainment\Wizard101</Mono></dd>
                <dt class="text-fg-muted">Revision</dt><dd><Mono size="13">r806919 (Wizard 1.610)</Mono></dd>
                <dt class="text-fg-muted">State</dt><dd><StateDot state="healthy" word="Found" /></dd>
            </dl>
        </Card>
        <Card title="Type dump">
            {#snippet footer()}
                <Button variant="action" icon="refresh-cw">Rebuild from the install</Button>
            {/snippet}
            <dl class="grid grid-cols-1 gap-8 text-13 sm:grid-cols-[auto_1fr]">
                <dt class="text-fg-muted">Built from</dt><dd><Mono size="13">r801440</Mono></dd>
                <dt class="text-fg-muted">State</dt><dd><StateDot state="waiting" word="Older than the install" /></dd>
            </dl>
        </Card>
    </div>
</div>
