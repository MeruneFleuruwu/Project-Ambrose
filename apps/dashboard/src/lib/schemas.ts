/*
 * Project Ambrose by Imjustchico
 * The shapes the panel accepts from the admin API, checked at the boundary with Valibot: the session, the app list, the status and the capabilities, each loose so a field a newer server adds is kept rather than refused, since the status schema only ever gains fields.
 */

import * as v from "valibot";

export const SessionAnswer = v.looseObject({
    app: v.string(),
    signed_in: v.boolean(),
    signed_in_with: v.nullable(v.picklist(["session", "token"])),
    csrf: v.nullable(v.string()),
    idle_seconds: v.number(),
    lifetime_seconds: v.number(),
});

export const AppEntry = v.looseObject({
    name: v.string(),
    role: v.string(),
    realm: v.string(),
    address: v.string(),
    port: v.number(),
    revision: v.string(),
});

export const AppList = v.array(AppEntry);

export const Problem = v.looseObject({
    code: v.string(),
    message: v.string(),
    subject: v.string(),
});

export const Status = v.looseObject({
    schema: v.number(),
    app: v.string(),
    role: v.string(),
    realm: v.string(),
    revision: v.string(),
    state: v.string(),
    uptime: v.number(),
    memory: v.nullable(v.looseObject({ resident_bytes: v.number() })),
    threads: v.nullable(v.number()),
    sessions: v.nullable(v.number()),
    tick: v.nullable(v.looseObject({ average_ms: v.number(), max_ms: v.number(), samples: v.number(), window_seconds: v.number() })),
    stats: v.record(v.string(), v.union([v.number(), v.string(), v.boolean()])),
    problems: v.array(Problem),
});

export const Capabilities = v.looseObject({
    schema: v.number(),
    reload_targets: v.array(v.string()),
    schedule_actions: v.array(v.string()),
    announcement_channels: v.array(v.string()),
    problem_codes: v.array(v.looseObject({ code: v.string(), description: v.string() })),
});

export type SessionAnswer = v.InferOutput<typeof SessionAnswer>;
export type AppEntry = v.InferOutput<typeof AppEntry>;
export type Problem = v.InferOutput<typeof Problem>;
export type Status = v.InferOutput<typeof Status>;
export type Capabilities = v.InferOutput<typeof Capabilities>;
