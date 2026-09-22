/*
 * Project Ambrose by Imjustchico
 * Starts a real Ambrose app whose admin API serves the built panel, for the end-to-end and screenshot runs: the patchserver the C++ build made, or the one AMBROSE_PANEL_APP names, on a port the caller picks with a known token and its logs in a folder of its own, waits until it answers, and stops it again.
 */

import { spawn, type ChildProcess } from "node:child_process";
import { existsSync, mkdtempSync, rmSync } from "node:fs";
import { tmpdir } from "node:os";
import path from "node:path";

export const token = "0123456789abcdef0123456789abcdef";

const candidates = [
    process.env.AMBROSE_PANEL_APP,
    "build/windows-msvc-x64/bin/Debug/patchserver.exe",
    "build/windows-msvc-x64/bin/RelWithDebInfo/patchserver.exe",
    "build/linux-gcc/bin/Debug/patchserver",
    "build/linux-gcc/bin/Release/patchserver",
];

export const app = candidates
    .filter((file): file is string => typeof file === "string" && file !== "")
    .map((file) => path.resolve(file))
    .find((file) => existsSync(file));

export const built = existsSync(path.resolve("apps/dashboard/dist/index.html"));

export type Panel = { url: string; stop: () => Promise<void> };

async function answers(url: string): Promise<boolean> {
    try {
        return (await fetch(`${url}/api/health`)).status === 401;
    } catch {
        return false;
    }
}

function exited(child: ChildProcess): Promise<void> {
    return new Promise((done) => {
        if (child.exitCode !== null || child.signalCode !== null) done();
        else child.once("exit", () => done());
    });
}

export async function startPanel(port: number): Promise<Panel> {
    if (!app) throw new Error("no built patchserver; build the C++ tree or set AMBROSE_PANEL_APP");
    const logs = mkdtempSync(path.join(tmpdir(), "ambrose-panel-"));
    const child = spawn(
        app,
        [
            "-c",
            path.resolve("src/server/apps/patchserver/patchserver.conf.dist"),
            "--set",
            "BindIP=127.0.0.1",
            "--set",
            "Admin.Enable=1",
            "--set",
            `Admin.Port=${port}`,
            "--set",
            `Admin.Token=${token}`,
            "--set",
            `Admin.DashboardDir=${path.resolve("apps/dashboard/dist")}`,
            "--set",
            `LogsDir=${logs}`,
        ],
        { stdio: "ignore" },
    );
    const url = `http://127.0.0.1:${port}`;
    const deadline = Date.now() + 20000;
    while (!(await answers(url))) {
        if (Date.now() > deadline || child.exitCode !== null) {
            child.kill();
            throw new Error(`the patchserver did not start its admin API on port ${port}`);
        }
        await new Promise((done) => setTimeout(done, 200));
    }
    return {
        url,
        stop: async () => {
            child.kill();
            await exited(child);
            rmSync(logs, { recursive: true, force: true });
        },
    };
}
