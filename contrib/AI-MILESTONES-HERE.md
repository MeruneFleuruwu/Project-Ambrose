<!-- Project Ambrose by Imjustchico: The prompt an outside contributor pastes into their own AI assistant to build one of the roadmap milestones that are open to outside help, with everything that assistant needs to know about this repository and the standard the work is held to. -->

# Start here, with your AI, to build a milestone

This is the harder door. contrib/AI-START-HERE.md is for the contributor track, which adds files in folders no milestone touches. This one is for the roadmap itself: real server code, in `src/`, judged against a milestone's own acceptance checks.

Everything inside the fence below is meant to be copied whole into any AI assistant, in one gesture. It is written as you speaking to that assistant. Paste it, answer its first question, and it has what it needs to work here without guessing. The short section after the fence is for you.

````
I am contributing to Project Ambrose, a Wizard101 server written from scratch in C++20 (github.com/Justchicoo/Project-Ambrose, MIT licensed). Help me finish one milestone from its roadmap, to the standard below. This is the project's own plan, not a side track: the code lands in `src/`, and the milestone is finished only when every acceptance check in its phase file is ticked with the evidence that proved it.

Read this whole prompt before you answer. Then ask me the questions at the end, and nothing before them.

## Read these before you plan anything

- **https://justchicoo.github.io/Project-Ambrose/state.json first, before anything else.** It is the project's live state, generated from the roadmap and the open pull requests, and it carries every milestone with a `status` of `landed`, `building`, `held`, `open`, `waiting` or `reserved`, plus what each needs, what it unlocks, who holds it and a `how_to_use` list of the rules. Fetch it, and take only a milestone whose status is `open`. If mine is anything else, stop and tell me what it says: a pull request for a held or claimed milestone is closed unread, and CI refuses the branch outright. The board a person reads is https://justchicoo.github.io/Project-Ambrose/ .
- `doc/MILESTONE-TRACK.md` - the rulebook behind that state: how one is claimed, what finishing means, what a milestone branch may change, and how holds work. A hold can cover a whole phase, so a milestone with every dependency built can still be closed to me.
- The phase file of my milestone, `doc/roadmap/phase-NN-*.md`, whole. Not only my milestone's section: the phase's **Review notes** at the top name faults the roadmap's own critic found, and the ones that name my milestone are mine to resolve.
- `doc/ARCHITECTURE.md` - the layering, the folder each subsystem belongs to, the file-header form per file type, the SQL update convention, and the settled Decisions. It is long; read the parts my milestone lands in.
- `CONTRIBUTING.md` and `doc/REVIEWING.md`. The second is the maintainer's own rulebook for judging this work, so it tells us exactly what will be checked and how.
- `doc/ROADMAP.md`'s "Where we are" paragraph, which says in one pass what actually runs today.
- Then only what my milestone needs: `doc/TOOLS.md`, `doc/CLIENT.md`, `doc/CAPTURE.md`, `doc/PATCHING.md`, `doc/PANEL.md`, `doc/config/*.md`, `doc/guides/logging.md`, `doc/guides/linux.md`.

## How a milestone is written, so you read it right

In the phase file my milestone is a section headed `## <id> <title>`, and it holds, in order: a one-line **Goal**; a **Size** and **Depends on** line; a short **Acceptance** list; then a detailed spec carrying **Deliverables** (the files to write, the tables to add, the settings to register), **Client messages**, **Data sources**, a fuller **Acceptance** list, and often **Risks**.

- The two Acceptance lists are the same checks at different detail. **Both get ticked.**
- The Deliverables list is binding. It names the files and the folders. If one of them is wrong or impossible, say so in the pull request and propose the change; do not quietly build something else, because the checks are written against those deliverables.
- A check that starts Dev-gated, Client-gated or Real client needs an installation, hardware or a client session. Some I can run, some I cannot.
- Everything the milestone depends on is already built, or the milestone would not be open to me. Everything after it is not. If my milestone seems to need something that does not exist, you have probably found a dependency the roadmap missed: say so rather than building it as well.

## The state of the project, so you assume neither more nor less than is true

- Phases 1 to 3 are built, and phase 4 has its core: 4.01 landed the world tick and the script and module loader. A real client, started by Ambrose's own launcher, reaches character select and stops. The session handshake, authentication, a wrong password and a retry, the character list and the shutdown notice are all that can be watched against Ambrose today. There is no world, no quest, no pet, no housing and no combat traffic yet, on either side.
- The pinned client revision is r806919 (Wizard 1.610). Read mine from `Bin/revision.dat` in my own install rather than copying an example.
- **These exist. Do not build them again.** `src/common/` has configuration, logging, threading, cryptography, encoding and utilities. `src/server/shared/Archives` reads KIWAD. `src/server/shared/Messages` loads the client's own message XML at run time into a registry. `src/server/shared/ObjectProperty` encodes and decodes both the compact wire format and the versionable BINd format, with a type registry, property enums, defaults, a JSON view and a fuzz test. `src/server/shared/ClientData` finds the install, keeps a per-revision type dump and extracts name tables. `src/server/shared/Network` has frames, the socket layer and the session handshake. `src/server/database` has the connection pool, the dated SQL updater and extraction. `src/server/game` has the world tick that owns the world thread, `ScriptMgr` and its hook classes, and `GameSession`, whose queue drains on that thread; a script or a module joins the build by existing, because CMake writes the loader from every `AddSC_` in `src/server/scripts` and every module under `modules/`. `ScriptLoader.h` lives in `src/server/scripts`, not in the scripting library, and `ScriptMgr::LoadScripts` takes the loader as an argument rather than reaching for it, so the hooks never depend on the content that uses them. `src/server/shared/Admin` serves the panel's API, and `apps/dashboard` is the panel. `src/tools` has `typeextract`, `bindecode`, `localetool`, `extractor`, `dbimport`, `launcher` and `template_extractor`.
- **These do not exist yet**, and a milestone that needs one is blocked rather than open: `CommandMgr` and security levels (4.02), the zone manager, the template store, and everything gameplay after them.
- The type dump `src/tools/typeextract` writes is version 2: an object with `version`, `revision`, `executable_sha256`, `extractor` and `classes`, keyed by class name, each class holding `name`, `bases`, `hash` and `properties`. It is built from my own client, into my own data folder, and is never committed.
- Tests are GoogleTest, in `src/test/` mirroring `src/`, in one executable run by CTest. A test that needs my installation carries the CTest label `client` and skips unless `AMBROSE_CLIENT_DIR` is set; one that needs my type dump reads `AMBROSE_TYPE_DUMP_PATH`; database tests run only when `AMBROSE_TEST_DB` holds a connection string such as `127.0.0.1;3306;root;root;ambrose_test`, and each creates uniquely named databases and drops them. `ctest` also runs the style and CI checks, so a green `ctest` is most of the review.
- A running server logs each message it refused or did not handle under `network.opcode`, but only up to `Network.DroppedMessageBurst = 64` per session and `Network.DroppedMessagesPerSecond = 16`, after which it adds a strike instead, and `Network.MaxStrikes = 10` closes the connection. A quiet log is not proof of quiet traffic.

## Hard rules. Breaking one closes the pull request, and each is there for a reason

1. **Paths.** My branch is named `milestone/<id>-<short-name>` and that name is what lets CI accept a change under `src/`. I may change the source tree, `data/sql/updates/`, `apps/` and `doc/`, plus **my own phase file and no other**. I may not touch `.github/`, `apps/ci/`, `apps/codestyle/`, `apps/progress/`, `doc/progress/`, `packages/ui/src/tokens/`, `README.md`, `CONTRIBUTING.md`, `CLAUDE.md`, `LICENSE`, `THIRD-PARTY-NOTICES.md`, `CMakePresets.json`, `vcpkg.json`, `.gitignore`, `doc/ROADMAP.md`, `doc/ARCHITECTURE.md`, `doc/REVIEWING.md`, `doc/CONTRIBUTOR-TRACK.md`, `doc/MILESTONE-TRACK.md`, `contrib/README.md`, `contrib/AI-START-HERE.md` or `contrib/AI-MILESTONES-HERE.md`. `python apps/ci/ci_contrib_paths.py --range upstream/main...HEAD --branch <my branch>` enforces exactly that. A new dependency in `vcpkg.json` is a proposal in the pull request, not a commit.
2. **No game data in the repository, ever.** That rule is why this repository can exist in public. No file from the client, no extracted asset, no dump, no capture, no run of hex or base64 pasted from one. `apps/ci/ci_forbidden_files.py` refuses `.wad`, `.nif`, `.kf`, `.kfm`, `.pcap` and `.pcapng`, any file beginning `KIWAD` or `BINd`, any JSON holding both `classes` and `version`, client protocol XML, and anything over 1,000,000 bytes. Offsets, field names, sizes, counts and hashes are facts about the data and are welcome; the bytes are not. Code reads my own installation at run time, behind an environment variable, and skips when it is absent. That is the line.
3. **Clean room.** Nothing copied, translated or ported from another Wizard101 server, emulator, wiki or site. Behaviour may be studied; code and text are written from scratch. AzerothCore may be studied for structure, which is where the layout comes from, and not for code.
4. **House style.** Every file I add starts with the branding header and a one-line brief of what it holds and does, and carries **no other comment anywhere**. The forms are exactly: C and C++, a block comment whose lines are ` * Project Ambrose by Imjustchico` then ` * <brief>`; CMake, shell, PowerShell, Python, YAML and conf, `# Project Ambrose by Imjustchico` then `# <brief>`; SQL, the same with `--`; Markdown, one HTML comment on line 1 reading `Project Ambrose by Imjustchico: <brief>`. The handle is the maintainer's and never changes to mine. JSON and binary files carry no header. Files are UTF-8 with no byte order mark, LF endings, no trailing whitespace, ending in a newline, ASCII unless the content is itself a translation. `python apps/codestyle/codestyle.py` is the judge. Write the brief as a sentence about what the file does, not a label: read a neighbouring file's header and match it.
5. **C++20, and the architecture as written.** The folder a subsystem belongs to, the layering order, dated SQL update files, content in the world database, custom content in scripts or modules rather than core edits. A milestone is not the place to re-litigate a settled Decision.
6. **Evidence names nothing personal.** This repository is public. A ticked check that quotes a path, account, address or machine name writes `<account>`, `<address>` and so on instead. What matters is the behaviour the line shows.
7. **Never** run KingsIsle's launcher or patcher against an install I want kept at a fixed revision, and never capture or contact KingsIsle's own servers. Everything here is done against my own installation and my own server.

## What finishing the milestone means

**Every acceptance check in both lists is ticked, in the same commit as the code that earns it, and each one quotes what proved it**, in brackets, the way the ticked ones already do:

```
- [x] Unit: yaw 0, pi/2, pi and 3pi/2 survive a byte round-trip within 1 byte step (MovementPackingTest.YawRoundTrip)
```

The name of the test that runs it, or the tool run and what it printed, or the screen and what it showed. **A check ticked by a test that does not exist ends the review immediately**, so before I push, grep the tree for every name you wrote in brackets and show me each one.

**A check I cannot run stays unticked.** If it needs an installation, a second machine, a client session or hardware I do not have, leave the box empty, name it in the pull request and say why. The work merges, the milestone stays open with what is left written next to it, and that is an honest outcome. Ticking a box we did not run is not, and it is the fastest way to have everything else in the branch distrusted.

**The shape of the work is not the work.** A header with no implementation, a test that asserts nothing, a table with no rows: each of those has been sent here before and none of them closed anything. If only part of the milestone is possible, build that part properly and say which part.

## How I want you to work

1. **Find out whether Ambrose already has it, before writing that it lacks it.** This is the single most common fault in contributions here: two proposals in a row set out to add what the project had already built. Read "Where we are", then the phase file, then grep `src/` for the type or the file name, then grep `src/test/` for a test that already proves it. The cheapest disproof of "nothing does X" is the test that does X, and it takes five minutes.
2. **Plan before writing.** What each acceptance check will be satisfied by, which file each deliverable lands in, and what the smallest failing test is. Put the cheapest experiment that could kill the approach first, so I do not spend a week on something wrong.
3. **Write the test before or with the code**, and make it fail first for the right reason. A check says the exact numbers to expect, such as packing (-2408.09, 2609.10, -7.13) landing within 4 units, or the first bytes of a written table. Assert those numbers, not a re-derivation of them, because a test that computes its own expectation proves only that the code agrees with itself.
4. **Verify by running, never by reading.** Build it, run the test, run the whole `ctest` suite, and run the tool on real input where there is one. Read the output rather than assuming it. When I paste output, read that too rather than agreeing with it.
5. **Make it fail usefully.** Name the file and the reason, return a typed error rather than a bare code, carry on past what can be skipped rather than losing a whole run to one unreadable input, and say what would make the output wrong. A decoder that reports zero problems on a corrupt file is worse than one that stops.
6. **Respect the limits already in the code.** Decoding is bounded by depth, object, list, memory and inflation limits read from settings; a new path through it keeps those bounds. New settings go in the app's `.conf.dist` with the same naming as its neighbours.
7. **Keep the diff to the milestone.** Renaming, reformatting or improving code on the way past makes the change unreviewable and is the most common reason a sound pull request is sent back. If you find a real bug outside the milestone, say so and leave it.
8. **Resolve the phase's review notes that name my milestone**, in the milestone or in the pull request, saying which and how.
9. Write the files, then have me run every check below and fix whatever they print.
10. Write the pull request description: the milestone id, what was built, how it was verified, which checks are ticked, which are not and why.

## Setting the whole thing up on my machine

Walk me through this once, step by step, waiting for what I actually see at each one. It ends with the servers, the panel and my own client running against each other, which is the setup the maintainer's own sessions work in. **Only the steps my milestone needs are worth doing first**, and the end of this section says which those are. None of it changes what I may take: the board decides that, and a machine set up beautifully gives me no claim on a held milestone.

**1. The toolchain.** CMake 3.25 or newer, vcpkg with `VCPKG_ROOT` set, and Visual Studio 2022+ or GCC 13+. Node 20+ only if I touch the panel, and a MySQL or MariaDB for anything that stores something.

```
cmake --preset windows-msvc-x64
cmake --build --preset windows-debug
ctest --preset windows-debug
```

On Linux the presets are `linux-gcc` and `linux-gcc-debug`, and `doc/guides/linux.md` is a guide somebody walked on Ubuntu 24.04. The first configure builds every dependency from source and takes about an hour; later ones are fast. The build is warnings-as-errors on both compilers, and MSVC and GCC disagree about what is a warning, so tell me which platform I built on and we say so in the pull request.

**2. The databases.** Three of them, `ambrose_login`, `ambrose_characters` and `ambrose_world`, created by the `dbimport` tool in the build's output folder rather than by hand. The default connection string for each is `127.0.0.1;3306;ambrose;ambrose;ambrose_login` and so on, meaning host, port, user, password, database, so the quickest start is a MySQL user named `ambrose` with password `ambrose` that may create databases. Anything else goes in `AMBROSE_LOGIN_DATABASE_INFO`, `AMBROSE_CHARACTER_DATABASE_INFO` and `AMBROSE_WORLD_DATABASE_INFO`, or in `dbimport.conf`. `dbimport` opens each pool once to prove it works, then exits 0, or 1 naming the first failure. The database tests are separate: they run only when `AMBROSE_TEST_DB` holds a connection string such as `127.0.0.1;3306;root;root;ambrose_test`, and each one makes uniquely named databases and drops them.

**3. The configuration files.** Every app needs its own `<app>.conf` next to the executable, copied from the `<app>.conf.dist` the build puts there. Started without one, an app exits naming the full path it wanted and the `.dist` to copy, so the error is the instruction. `doc/config/README.md` has the file format and the layers, and `doc/config/<app>.md` documents every option that app takes, because the files themselves carry no comments beyond their header.

**4. The client data, which sets itself up.** On a first start with `Setup.Mode = auto`, which is the default, a game server finds the newest Wizard101 installation on my machine, builds its type dump by emulating the client's own program, extracts the name tables, and saves the choice to `conf.d/client-data.conf`. `AMBROSE_CLIENT_DIR` names the install explicitly: the folder holding `Bin` and `Data`. The dump lands in the Ambrose data folder, `%LOCALAPPDATA%/ProjectAmbrose` on Windows or `~/.local/share/project-ambrose` otherwise, as `types/<revision>.json`, and is never committed. **Never run KingsIsle's launcher or patcher against that install**: it moves the revision under me and every recorded fact about it.

**5. The servers.** Either start them one at a time, the login server on port 12000 and the game server on 12333, or start `supervisor`, which runs the login, game and patch servers in order, takes back the ones still running if it is restarted, and captures each one's output. `ctest` already proves the apps reach readiness and shut down cleanly against disposable databases, so a failure here is usually configuration rather than code.

**6. The panel.** In `supervisor.conf` set `Panel.Enable = 1`. It listens on `Panel.Port`, 12080, which is deliberately clear of the 12000 the login server takes for game clients on the same machine. `supervisor --panel-self-signed` writes a certificate that signs itself and prints its fingerprint, which is what a panel on my own machine wants. On its first start with no operator, the panel prints a one-time link, once, good for thirty minutes and only from the machine it runs on, that makes the owner account with a name and password of my choosing, so no default password ever exists. If I clear the console before using it, it is in the log rather than gone. The panel serves the built page from `Panel.DashboardDir`, which defaults to a `dashboard` folder beside the executable:

```
npm install
npm run build --workspace apps/dashboard
```

Then point `Panel.DashboardDir` at `apps/dashboard/dist`. For work on the panel's own page there is a development server instead, which proxies its API calls to a running panel:

```
AMBROSE_PANEL_API=https://127.0.0.1:12080 npm run dev --workspace apps/dashboard
```

**7. My own client.** The `launcher` tool starts my own installation against my own login server, always with patching off, from a folder of its own, and never through KingsIsle's launcher: `launcher --client <the install folder>`. It needs Windows. With a login server running, that is the whole loop: my client reaches the login screen, authenticates against my server, and lands on character select.

**What my milestone actually needs.** Ask me for only these. A milestone whose checks are unit tests needs nothing past step 1. Anything that stores something adds step 2. Anything reading the client's own archives, dumps or zones adds step 4 and my installation. A check marked Real client needs steps 5 and 7 and somebody at the keyboard, and a check marked Dev-gated may need a second machine or hardware I do not have, which stays unticked and is named in the pull request. Panel milestones are held by the maintainer's own sessions, so step 6 is for watching the thing run, not for work I may take.

A branch named `milestone/<id>-<short-name>` builds the Linux GCC leg in CI by itself, so an open pull request tells us both whether it compiles there, and the maintainer adds a label for the Windows leg when it is worth one. The first run from a new contributor waits for a maintainer to approve it.

## Before the pull request

Have me commit first, because these read committed work, then run these from the repository root (`python` may be `py` on Windows):

```
git fetch upstream
curl -s https://justchicoo.github.io/Project-Ambrose/state.json | python -c "import json,sys; print([m for m in json.load(sys.stdin)['milestones'] if m['id']=='<my milestone>'])"
python apps/ci/ci_contrib_paths.py --range upstream/main...HEAD --branch <my branch>
python apps/codestyle/codestyle.py
python apps/ci/ci_forbidden_files.py
ctest --preset windows-debug
git status --porcelain
```

That first line is the board again: my milestone should still say `building` with my name on it. If it now says `held`, the maintainer's own sessions have taken that area since I started, and I should stop and ask in the Discord rather than push into it.

Three dots, and the remote branch my pull request targets, never a local `main`, because a stale or moved-on `main` makes that check flag files I never touched. `upstream` is whichever of my remotes is github.com/Justchicoo/Project-Ambrose; a clone of my own fork has none until I add it with `git remote add upstream https://github.com/Justchicoo/Project-Ambrose.git`. `git status` must be clean: an extracted file, a dump or a generated database file left in the tree is the thing rule 2 exists to stop, and several milestones generate exactly those.

Every commit on the branch needs a trailer naming you, such as `Co-Authored-By: <your model name> <noreply@example.com>`; the checker fails any commit in the range without one, not only the last.

## One milestone per branch, claimed before it is built

```
git fetch upstream
git switch --detach upstream/main
git switch -c milestone/<id>-<short-name>
```

Always from `upstream/main`, never from another branch that has an open pull request, because that turns two independent contributions into a chain where revising the first breaks the second.

**Open the pull request as a draft on the first day, before the work is done.** That is what reserves the milestone, and nobody has to be told: the board reads the open pull requests, so within minutes it shows my milestone as being built, by me, and stops anybody else taking it. Building for a week in silence risks somebody else landing it first. Title it `<id> <what you are building>`.

A claim is not forever. Fourteen days with no push and the board puts the milestone back on the open list, with whatever I pushed left in place, so somebody else can carry it. A push is all it takes to keep it.

Check before opening it: `git log --oneline upstream/main..HEAD` shows only this milestone's commits, and `git diff --name-only upstream/main...HEAD` only its files.

After a merge the branch holds nothing git can apply again, because pull requests are squashed: `git checkout main`, `git reset --hard upstream/main`, `git push --force origin main`, and branch again from there.

## Keep me honest

If I tell you something I only remember or assume, mark it unproven rather than writing it as a fact. If a test fails, tell me what actually happened rather than adjusting the test until it passes, and never weaken or skip an existing test to get a green run: that is the one change nobody here will merge. If the milestone turns out to need something that does not exist yet, say so and we tell the maintainer rather than building it too. Questions go to the project's Discord, https://discord.gg/Dx6ACDUj6N, or into the pull request itself.

## Now ask me

1. Which milestone id am I taking, and is it in doc/MILESTONE-TRACK.md's "Open now" table?
2. What do I have already: Windows or Linux, a working build, a MySQL or MariaDB, my own client installation, a type dump? Anything missing that my milestone needs, walk me through the matching step of the setup section before we plan the work, and skip the steps it does not need.
3. If I am unsure which to take, recommend one that fits what I have, and say which of its acceptance checks I will not be able to run.
````

## After you paste it

Answer its questions, then read `doc/MILESTONE-TRACK.md` yourself before it starts. Two things there decide whether the evening is wasted: the milestone has to be in the "Open now" table, and the branch has to be named `milestone/<id>-<short-name>`, because that name is the only reason CI lets the change touch `src/`.

Open the pull request as a draft on the first day. It is what holds the milestone for you.

If what you want to build is not on the roadmap at all, or the milestone you want is reserved, ask in the Discord rather than sending it: https://discord.gg/Dx6ACDUj6N. The other door, contrib/AI-START-HERE.md, is open for tools, findings, guides, fixtures and proposals, and nothing there can collide with a milestone.
