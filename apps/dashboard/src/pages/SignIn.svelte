<!-- Project Ambrose by Imjustchico: The page a signed-out browser sees: one field for the app's admin token, traded once for a browser session so the token never stays in the browser, with a field's own problem beside it, any other refusal above the form with its request id, and a note when a session has just ended. -->
<script lang="ts">
    import * as Card from "$lib/components/ui/card/index.js";
    import { Button } from "$lib/components/ui/button/index.js";
    import { Input } from "$lib/components/ui/input/index.js";
    import { Label } from "$lib/components/ui/label/index.js";
    import { ApiError, session, signIn } from "$lib/api.svelte.js";
    import CircleAlertIcon from "@lucide/svelte/icons/circle-alert";
    import LogInIcon from "@lucide/svelte/icons/log-in";

    let token = $state("");
    let busy = $state(false);
    let fieldProblem = $state("");
    let problem = $state<ApiError | null>(null);
    let others = $state<[string, string][]>([]);
    let requestId = $state("");

    function describe(error: ApiError): string {
        if (error.code === "wrong_token") return "That is not this app's admin token.";
        if (error.status === 429) return "Too many wrong tokens from this address. Wait a moment and try again.";
        if (error.status === 403)
            return "The server refused a sign-in from this page's address. Open the panel from the address the app prints.";
        if (error.status === 0) return "The panel could not reach its server. Check that the app is still running.";
        return error.message;
    }

    async function submit(event: SubmitEvent) {
        event.preventDefault();
        fieldProblem = token.trim() === "" ? "Enter this app's admin token." : "";
        problem = null;
        others = [];
        requestId = "";
        if (fieldProblem !== "") return;
        busy = true;
        try {
            await signIn(token.trim());
            token = "";
        } catch (failure) {
            if (failure instanceof ApiError) {
                fieldProblem = failure.fields.token ?? "";
                others = Object.entries(failure.fields).filter(([field]) => field !== "token");
                problem = Object.keys(failure.fields).length === 0 ? failure : null;
                requestId = failure.requestId;
            } else {
                problem = new ApiError(0, "failed", "Signing in failed for a reason the panel could not read.", "");
            }
        } finally {
            busy = false;
        }
    }
</script>

<main class="flex min-h-svh items-center justify-center bg-background p-4">
    <Card.Root class="w-full max-w-sm shadow-xs">
        <Card.Header>
            <div
                class="mb-2 flex size-10 items-center justify-center rounded-lg bg-primary font-serif text-xl font-bold text-primary-foreground"
            >
                A
            </div>
            <Card.Title class="font-serif text-2xl"><h1>Sign in to Ambrose</h1></Card.Title>
            <Card.Description>
                Enter the admin token this app keeps. It is traded once for a session in this browser and is never stored here.
            </Card.Description>
        </Card.Header>
        <Card.Content>
            <form class="space-y-4" onsubmit={submit} novalidate>
                {#if session.ended}
                    <p class="rounded-md border px-3 py-2 text-sm text-muted-foreground" role="status">
                        Your session ended. Sign in again to carry on.
                    </p>
                {/if}
                {#if problem || others.length > 0}
                    <div class="flex gap-2 rounded-md border border-destructive/30 bg-destructive/5 px-3 py-2 text-sm" role="alert">
                        <CircleAlertIcon class="mt-0.5 size-4 shrink-0 text-destructive" />
                        <div class="space-y-1">
                            {#if problem}<p>{describe(problem)}</p>{/if}
                            {#each others as [field, text] (field)}
                                <p><span class="font-mono">{field}</span>: {text}</p>
                            {/each}
                            {#if requestId}
                                <p class="text-xs text-muted-foreground">Request <span class="font-mono select-all">{requestId}</span></p>
                            {/if}
                        </div>
                    </div>
                {/if}
                <div class="space-y-2">
                    <Label for="admin-token">Admin token</Label>
                    <Input
                        id="admin-token"
                        type="password"
                        autocomplete="off"
                        spellcheck="false"
                        bind:value={token}
                        aria-invalid={fieldProblem !== ""}
                        aria-describedby={fieldProblem !== "" ? "admin-token-problem" : undefined}
                        class="font-mono"
                    />
                    {#if fieldProblem !== ""}
                        <p id="admin-token-problem" class="text-sm text-destructive">{fieldProblem}</p>
                        {#if requestId && !problem && others.length === 0}
                            <p class="text-xs text-muted-foreground">Request <span class="font-mono select-all">{requestId}</span></p>
                        {/if}
                    {/if}
                </div>
                <Button type="submit" class="w-full" disabled={busy}><LogInIcon />{busy ? "Signing in" : "Sign in"}</Button>
            </form>
        </Card.Content>
        <Card.Footer>
            <p class="text-xs text-muted-foreground">
                The token is <span class="font-mono">Admin.Token</span> in the app's config, or the token file the app names in its log when it
                generates one.
            </p>
        </Card.Footer>
    </Card.Root>
</main>
