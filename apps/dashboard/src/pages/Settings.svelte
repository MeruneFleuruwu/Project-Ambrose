<!-- Project Ambrose by Imjustchico: The settings page: a few of each app's settings in groups, each saved with a reload, the form 17.13 turns into the full editor. -->
<script lang="ts">
    import { Button, Card, Heading, Select, Switch, TextField } from "@ambrose/ui";
</script>

<div class="flex flex-col gap-20">
    <Heading level={1}>Settings</Heading>
    <Select id="settings-app" label="App" items={[{ value: "gameserver", label: "gameserver" }, { value: "loginserver", label: "loginserver" }]} value="gameserver" class="max-w-320" />
    <div class="grid grid-cols-1 gap-16 lg:grid-cols-2">
        <Card title="Realm">
            <div class="flex flex-col gap-12">
                <TextField id="realm-name" label="Realm name" value="Ambrose" />
                <TextField id="realm-limit" label="Player limit" type="number" value="500" hint="Applies from the next realm list refresh." />
            </div>
        </Card>
        <Card title="Admin API">
            <div class="flex flex-col gap-12">
                <TextField id="admin-bind" label="Listen address" mono value="127.0.0.1" />
                <TextField id="admin-token" label="Token" type="password" mono value="********" hint="Secret: never shown or logged." />
                <Switch id="admin-enable" label="Serve the admin API" checked />
            </div>
        </Card>
    </div>
    <div class="flex flex-wrap gap-8">
        <Button variant="action" icon="check">Save and reload</Button>
        <Button variant="ghost">Discard changes</Button>
    </div>
</div>
