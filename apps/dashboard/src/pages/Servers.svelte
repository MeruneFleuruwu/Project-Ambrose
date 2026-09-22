<!-- Project Ambrose by Imjustchico: The servers page: every app in one table with its state, place, uptime and build, and the power buttons that act on all of them at once. -->
<script lang="ts">
    import { Button, DataTable, Heading } from "@ambrose/ui";
    import { apps } from "../sample";

    const rows = apps.map((app) => ({ name: app.name, role: app.role, state: app.word, address: `${app.address}:${app.port}`, uptime: app.uptime || "Not running", revision: app.revision }));
    const columns = [
        { id: "name", header: "App", value: (row: (typeof rows)[number]) => row.name, sortable: true },
        { id: "role", header: "Role", value: (row: (typeof rows)[number]) => row.role },
        { id: "state", header: "State", value: (row: (typeof rows)[number]) => row.state, sortable: true },
        { id: "address", header: "Address", value: (row: (typeof rows)[number]) => row.address, mono: true },
        { id: "uptime", header: "Uptime", value: (row: (typeof rows)[number]) => row.uptime },
        { id: "revision", header: "Build", value: (row: (typeof rows)[number]) => row.revision, mono: true },
    ];
</script>

<div class="flex flex-col gap-20">
    <div class="flex flex-wrap items-center justify-between gap-12">
        <Heading level={1}>Servers</Heading>
        <div class="flex flex-wrap gap-8">
            <Button variant="action" icon="play">Start all</Button>
            <Button variant="quiet" icon="rotate-ccw">Restart all</Button>
            <Button variant="danger" icon="power">Stop all</Button>
        </div>
    </div>
    <DataTable caption="Every app this panel runs" {columns} {rows} />
</div>
