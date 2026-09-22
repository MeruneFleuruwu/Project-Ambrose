<!-- Project Ambrose by Imjustchico: The servers page: every app in one table with its state, place, uptime and build, a menu of actions per row, and the power buttons that act on all of them at once, each answered with a toast while the data is a sample. -->
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
    import { openFor } from "../focus.svelte";
    import { requestPower } from "../power";
    import { apps } from "../sample";
</script>

<PageHeader title="Servers" description="Start, stop and restart the apps this panel runs.">
    {#snippet actions()}
        <Button variant="outline" class="text-destructive hover:text-destructive" onclick={() => requestPower("stop", "every app")}
            ><PowerIcon />Stop all</Button
        >
        <Button variant="outline" onclick={() => requestPower("restart", "every app")}><RotateCcwIcon />Restart all</Button>
        <Button onclick={() => requestPower("start", "every app")}><PlayIcon />Start all</Button>
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
                                    <Button variant="ghost" size="icon" class="size-8" aria-label={`Actions for ${app.name}`} {...props}
                                        ><EllipsisIcon /></Button
                                    >
                                {/snippet}
                            </DropdownMenu.Trigger>
                            <DropdownMenu.Content align="end">
                                {#if app.state === "unknown"}
                                    <DropdownMenu.Item onSelect={() => requestPower("start", app.name)}>Start</DropdownMenu.Item>
                                {:else}
                                    <DropdownMenu.Item onSelect={() => requestPower("restart", app.name)}>Restart</DropdownMenu.Item>
                                {/if}
                                <DropdownMenu.Item onSelect={() => openFor(app.name, "logs")}>Open logs</DropdownMenu.Item>
                                <DropdownMenu.Item onSelect={() => openFor(app.name, "console")}>Open console</DropdownMenu.Item>
                                {#if app.state !== "unknown"}
                                    <DropdownMenu.Separator />
                                    <DropdownMenu.Item variant="destructive" onSelect={() => requestPower("stop", app.name)}
                                        >Stop</DropdownMenu.Item
                                    >
                                {/if}
                            </DropdownMenu.Content>
                        </DropdownMenu.Root>
                    </Table.Cell>
                </Table.Row>
            {/each}
        </Table.Body>
    </Table.Root>
</Card.Root>
