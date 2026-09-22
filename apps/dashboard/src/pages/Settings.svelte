<!-- Project Ambrose by Imjustchico: The settings page: each app's settings in tabs by area, each saved with a live reload, the form 17.13 turns into the full editor. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import * as Tabs from "$lib/components/ui/tabs/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import { Input } from "$lib/components/ui/input/index.js";
    import { Label } from "$lib/components/ui/label/index.js";
    import { Switch } from "$lib/components/ui/switch/index.js";
    import PageHeader from "../components/PageHeader.svelte";
</script>

<PageHeader title="Settings" description="Changes apply live with a reload; nothing needs a restart.">
    {#snippet actions()}
        <Button variant="ghost">Discard</Button>
        <Button>Save and reload</Button>
    {/snippet}
</PageHeader>

<Tabs.Root value="realm" class="gap-4">
    <Tabs.List>
        <Tabs.Trigger value="realm">Realm</Tabs.Trigger>
        <Tabs.Trigger value="admin">Admin API</Tabs.Trigger>
        <Tabs.Trigger value="logging">Logging</Tabs.Trigger>
    </Tabs.List>
    <Tabs.Content value="realm">
        <Card.Root class="shadow-xs">
            <Card.Header>
                <Card.Title>Realm</Card.Title>
                <Card.Description>What players see in the realm list.</Card.Description>
            </Card.Header>
            <Card.Content class="grid gap-6 md:grid-cols-2">
                <div class="grid content-start gap-2">
                    <Label for="realm-name">Realm name</Label><Input id="realm-name" value="Ambrose" />
                </div>
                <div class="grid content-start gap-2">
                    <Label for="realm-limit">Player limit</Label>
                    <Input id="realm-limit" type="number" value="500" />
                    <p class="text-xs text-muted-foreground">Applies from the next realm list refresh.</p>
                </div>
                <div class="flex items-center justify-between rounded-lg border p-4 md:col-span-2">
                    <div class="space-y-0.5">
                        <Label for="realm-recommended">Recommended</Label>
                        <p class="text-xs text-muted-foreground">Show this realm first to new players.</p>
                    </div>
                    <Switch id="realm-recommended" checked />
                </div>
            </Card.Content>
        </Card.Root>
    </Tabs.Content>
    <Tabs.Content value="admin">
        <Card.Root class="shadow-xs">
            <Card.Header>
                <Card.Title>Admin API</Card.Title>
                <Card.Description>The listener this panel talks to.</Card.Description>
            </Card.Header>
            <Card.Content class="grid gap-6 md:grid-cols-2">
                <div class="grid content-start gap-2">
                    <Label for="admin-bind">Listen address</Label><Input id="admin-bind" value="127.0.0.1" class="font-mono" />
                </div>
                <div class="grid content-start gap-2">
                    <Label for="admin-token">Token</Label>
                    <Input id="admin-token" type="password" value="hidden-token" class="font-mono" />
                    <p class="text-xs text-muted-foreground">A secret: never shown or logged.</p>
                </div>
                <div class="flex items-center justify-between rounded-lg border p-4 md:col-span-2">
                    <div class="space-y-0.5">
                        <Label for="admin-enable">Serve the admin API</Label>
                        <p class="text-xs text-muted-foreground">Turning this off closes the listener at once.</p>
                    </div>
                    <Switch id="admin-enable" checked />
                </div>
            </Card.Content>
        </Card.Root>
    </Tabs.Content>
    <Tabs.Content value="logging">
        <Card.Root class="shadow-xs">
            <Card.Header>
                <Card.Title>Logging</Card.Title>
                <Card.Description>How much each server writes and keeps.</Card.Description>
            </Card.Header>
            <Card.Content class="grid gap-6 md:grid-cols-2">
                <div class="grid content-start gap-2">
                    <Label for="log-backlog">Live backlog</Label><Input id="log-backlog" type="number" value="1000" />
                </div>
                <div class="grid content-start gap-2">
                    <Label for="log-days">Keep log files for (days)</Label><Input id="log-days" type="number" value="14" />
                </div>
            </Card.Content>
        </Card.Root>
    </Tabs.Content>
</Tabs.Root>
