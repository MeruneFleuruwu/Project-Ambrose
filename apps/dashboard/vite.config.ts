/*
 * Project Ambrose by Imjustchico
 * How the panel is built: a relative base so the identical output serves from a subpath or a web view, the shared plugin set so its tokens and icons are the same ones the launcher uses, and the $lib alias the shadcn-svelte components are written against.
 */

import { fileURLToPath } from "node:url";
import { defineConfig } from "vite";
import { ambrosePlugins } from "../../packages/ui/vite.ts";

export default defineConfig({
    base: "./",
    plugins: ambrosePlugins(),
    resolve: {
        alias: { $lib: fileURLToPath(new URL("./src/lib", import.meta.url)) },
    },
    build: {
        outDir: "dist",
        emptyOutDir: true,
        target: "es2022",
        sourcemap: false,
    },
});
