<!-- Project Ambrose by Imjustchico: The servers page: every app in one table with its state, place, uptime and build, a menu of actions per row, and the power buttons that act on all of them at once. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import * as DropdownMenu from "$lib/components/ui/dropdown-menu/index.js";
    import * as Table from "$lib/components/ui/table/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import EllipsisIcon from "@lucide/svelte/icons/ellipsis";
    import PlayIcon from "@lucide/svelte/icons/play";
    import PowerIcon from "@lucide/svelte/icons/power";
    import RotateCcwIcon from "@lucide/svelte/icons/rotate-ccw";
    import PageHeader from "../components/PageHeader.svelte";
    import StatusBadge from "../components/StatusBadge.svelte";
    import { apps } from "../sample";
</script>

<PageHeader title="Servers" description="Start, stop and restart the apps this panel runs.">
    {#snippet actions()}
        <Button variant="outline" class="text-destructive hover:text-destructive"><PowerIcon />Stop all</Button>
        <Button variant="outline"><RotateCcwIcon />Restart all</Button>
        <Button><PlayIcon />Start all</Button>
    {/snippet}
</PageHeader>

<Card.Root class="py-0 shadow-xs">
    <Table.Root>
        <Table.Header>
            <Table.Row class="hover:bg-transparent">
                <Table.Head class="pl-6">App</Table.Head>
                <Table.Head>State</Table.Head>
                <Table.Head class="hidden md:table-cell">Address</Table.Head>
                <Table.Head class="hidden sm:table-cell">Uptime</Table.Head>
                <Table.Head class="hidden lg:table-cell">Build</Table.Head>
                <Table.Head class="w-12 pr-6"><span class="sr-only">Actions</span></Table.Head>
            </Table.Row>
        </Table.Header>
        <Table.Body>
            {#each apps as app (app.name)}
                <Table.Row>
                    <Table.Cell class="pl-6">
                        <div class="font-medium">{app.name}</div>
                        <div class="text-xs text-muted-foreground">{app.role}{app.realm ? ` · ${app.realm}` : ""}</div>
                    </Table.Cell>
                    <Table.Cell><StatusBadge tone={app.state}>{app.word}</StatusBadge></Table.Cell>
                    <Table.Cell class="hidden font-mono text-xs md:table-cell">{app.address}:{app.port}</Table.Cell>
                    <Table.Cell class="hidden sm:table-cell">{app.uptime || "Not running"}</Table.Cell>
                    <Table.Cell class="hidden font-mono text-xs lg:table-cell">{app.revision}</Table.Cell>
                    <Table.Cell class="pr-6">
                        <DropdownMenu.Root>
                            <DropdownMenu.Trigger>
                                {#snippet child({ props })}
                                    <Button variant="ghost" size="icon" class="size-8" aria-label={`Actions for ${app.name}`} {...props}><EllipsisIcon /></Button>
                                {/snippet}
                            </DropdownMenu.Trigger>
                            <DropdownMenu.Content align="end">
                                <DropdownMenu.Item>Restart</DropdownMenu.Item>
                                <DropdownMenu.Item>Open logs</DropdownMenu.Item>
                                <DropdownMenu.Item>Open settings</DropdownMenu.Item>
                                <DropdownMenu.Separator />
                                <DropdownMenu.Item variant="destructive">Stop</DropdownMenu.Item>
                            </DropdownMenu.Content>
                        </DropdownMenu.Root>
                    </Table.Cell>
                </Table.Row>
            {/each}
        </Table.Body>
    </Table.Root>
</Card.Root>
