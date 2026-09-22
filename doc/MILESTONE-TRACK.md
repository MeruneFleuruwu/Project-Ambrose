<!-- Project Ambrose by Imjustchico: The second track other people work from, the roadmap itself: which milestones are open to outside help, how one is taken so two people never build the same thing, what finishing one means, and what a review holds it to. -->

# Milestone track

doc/CONTRIBUTOR-TRACK.md is the safe track: its own folders, nothing a milestone touches. This is the other one. A named set of milestones from doc/ROADMAP.md is open to outside help, with the source tree, the tests and the acceptance checks that come with them.

It exists because the phases are the project, and the maintainer's own agents build them one at a time in dependency order. Every milestone finished from outside is one the project does not have to wait for. What it costs is the risk this document is written to remove: two people on the same milestone, half a milestone that cannot be judged, a change that lands in a file another milestone is being built in right now.

Working with an AI assistant is expected here. contrib/AI-MILESTONES-HERE.md is a prompt to paste into yours; it carries what it needs to know before it writes a line. Ask in the Discord first if anything is unclear: https://discord.gg/Dx6ACDUj6N.

## Only the milestones named below

**Open now** is the whole list. A milestone that is not in it is reserved, whatever its dependencies say, because it is being built right now, it is next in the maintainer's own queue, or its acceptance can only be run on the maintainer's machine. A pull request for a reserved milestone is closed, and that is a waste of your evening, so take one from the table or ask in the Discord for another to be opened.

`python apps/progress/ready.py` prints every milestone whose dependencies are all finished and marks each one from this document's tables, counting anything it does not name as reserved, so the tool and this page can never drift apart. `--open` narrows it to the ones nobody holds, `--blocked` says what is waiting and on what.

## Taking one

1. Say in the Discord which one you are taking, or open the pull request as a draft straight away. Whoever opens a draft first holds it.
2. Branch from `upstream/main`, and name the branch `milestone/<id>-<short-name>`, such as `milestone/4.04-world-wire-math`. The name is not decoration: `apps/ci/ci_contrib_paths.py` reads it, and it is the only reason CI lets the change touch `src/`.
3. Open the pull request early, as a draft, titled `<id> <what you are building>`. That is what reserves it. The maintainer moves the row into **In flight** with your name on it.
4. One milestone per pull request. Never build a second milestone's branch on the first one's.

If you go quiet for two weeks the row goes back to **Open now**, with whatever you pushed left in place, so somebody else can carry it.

## What finishing one means

A milestone is finished when **every acceptance check in its phase file is ticked**, in the same commit as the code that earns them, and not before. Most milestones carry two lists: the short one under the milestone heading and the full one at the end of the detailed spec. Both are the same checks in different detail, and both get ticked.

A ticked check quotes what proved it, in brackets, the way the ones already ticked do:

```
- [x] Unit: yaw 0, pi/2, pi and 3pi/2 survive a byte round-trip within 1 byte step (MovementPackingTest.YawRoundTrip)
```

The name of the test that runs it, the tool run and what it printed, or the screen and what it showed. Evidence names nothing personal: a real account, address, path or machine name is written `<account>`, `<address>` and so on. A check with no evidence in brackets is not ticked, and a check ticked by a test that does not exist is the one thing that ends a review immediately.

**A check you cannot run stays unticked.** Some are labelled Dev-gated, Client-gated or Real client, and need an installation, a second machine or hardware you may not have. Leave those boxes empty, say in the pull request exactly which ones and why, and send the rest. The work merges, the milestone stays open, and the row moves to **In flight** with what is left written next to it. That is an honest, welcome outcome. Ticking a box you did not run is not.

**Build what the milestone says, not around it.** The deliverables list under the detailed spec names the files to write, the tables to add and the client messages involved. If one of them is wrong or impossible, say so in the pull request and propose the change. Do not quietly build something else: the acceptance checks are written against those deliverables and a review reads them together.

## What a milestone branch may change

Everything under `src/`, `data/sql/updates/`, `apps/` and `doc/` except the list below, plus its own phase file. The check enforces exactly that:

```
python apps/ci/ci_contrib_paths.py --range upstream/main...HEAD --branch milestone/<id>-<short-name>
```

It refuses another phase's file, so a change that needs one is a change of scope, and it refuses these, which the maintainer keeps so that concurrent work never collides in them: `.github/`, `apps/ci/`, `apps/codestyle/`, `apps/progress/`, `doc/progress/`, `packages/ui/src/tokens/`, `README.md`, `CONTRIBUTING.md`, `CLAUDE.md`, `LICENSE`, `THIRD-PARTY-NOTICES.md`, `CMakePresets.json`, `vcpkg.json`, `.gitignore`, `doc/ROADMAP.md`, `doc/ARCHITECTURE.md`, `doc/REVIEWING.md`, `doc/CONTRIBUTOR-TRACK.md`, `doc/MILESTONE-TRACK.md`, `contrib/README.md`, `contrib/AI-START-HERE.md` and `contrib/AI-MILESTONES-HERE.md`.

`doc/ROADMAP.md`'s "Where we are" and the progress card are written by the maintainer when the milestone lands, from the boxes you ticked. A new dependency in `vcpkg.json` is a proposal in the pull request, not a commit.

## What every milestone pull request needs

- **It builds and its tests pass on at least one platform, and you say which.** `cmake --preset windows-msvc-x64` then `cmake --build --preset windows-debug` and `ctest --preset windows-debug`, or `linux-gcc` with `linux-gcc-debug`. The first configure builds every dependency from source and takes about an hour. `ctest` also runs the style and CI checks, so a green `ctest` is most of the review.
- **New tests live in `src/test/`, mirroring the folder of the code they test**, and are named in the acceptance check they prove. A test that needs an installation carries the CTest label `client` and skips unless `AMBROSE_CLIENT_DIR` is set; one that needs the user's own type dump reads `AMBROSE_TYPE_DUMP_PATH`. Never make an existing test optional to get it passing.
- **C++20, and the architecture as written.** doc/ARCHITECTURE.md's layering, its folder for each subsystem, dated SQL update files, content in the world database, and the settled Decisions. A milestone is not the place to re-litigate one.
- **The branding header and no other comment**, in the form doc/ARCHITECTURE.md gives for the file type. `python apps/codestyle/codestyle.py` is the judge. Files are UTF-8 with no byte order mark, LF endings, no trailing whitespace, ending in a newline, ASCII unless the content is a translation.
- **No file from the game client, and nothing generated from one.** Not an archive, an asset, a dump, a capture or a run of bytes pasted from one. A tool reads the user's own installation at run time; that is the line, and `python apps/ci/ci_forbidden_files.py` guards it.
- **Written from scratch.** Another server's behaviour may be studied. Its code and its data may not be copied, translated or ported.
- **A commit trailer naming the AI that wrote it**, on every commit in the branch, such as `Co-Authored-By: <model name> <noreply@example.com>`.
- **A description that says what was built, how it was verified, which checks are ticked and which are not.** Unverified work is not merged.

## How it is reviewed

doc/REVIEWING.md is the rulebook, and its first line applies hardest here: verified by running, never by reading. Expect the maintainer to build the branch, run its tests, run the ones it claims by name, and try the failure the code says it handles. A branch named for a milestone builds the Linux GCC leg in CI by itself, without waiting for a label, and the maintainer adds a `ci:` label for the Windows leg or the sanitizers when the change deserves them.

Then one of four things happens, each with one message saying which and why: it merges and the milestone is marked landed; it merges with the milestone left open because gated checks remain; it merges and the maintainer fixes what review found on `main`, with you kept as co-author; or it is closed with the reason and what would make it mergeable.

## Open now

| ID | Milestone | Size | What you need | Why it is a good one to take |
|---|---|---|---|---|
| 1.06, 1.07, 1.08 | ByteBuffer and DML primitives, BitReader/BitWriter, and the text encodings | S | Your own client installation | One optional integration test, decoding the UTF-16 text of a single `Locale/*.lang` entry from your own `Root.wad`, is the last unticked check of all three milestones. The smallest way to finish three at once, on a branch named `milestone/1.06-lang-round-trip` |
| 3.18 | Updater part 2: rehash, rename, dead refs, pending, modules | M | A build and a MySQL or MariaDB | Needs no client at all. Four unit and integration checks about the SQL updater's own behaviour, each one a named case with the expected error |
| 4.04 | World wire math and LocationString | S | A build; one check reads your client's XML | Self-contained maths and parsing with eleven checks that say the exact numbers. Everything the world phase does with positions rests on it |
| 4.08 | Zone extractor part 1: WizZoneData | M | Your own client installation | The largest one open, and the one that unblocks most: zone templates, locations and objects read out of your own install into the world database. Its phase file calls it oversized, so landing the extractor and its reporting first, with the row checks after, is expected |
| 5.07 | Binary type-registry cache | S | Your own client installation | A converter and a loader that make startup fast, with a stale cache rejected. Three checks, all mechanical |
| 6.09 | Schema probe for classes missing from the dump | M | Your own client installation | Names the classes the client's own dump does not describe, by sweeping archives and reading hashes. The check lists the exact hashes and counts to reproduce |
| 8.14 | Versionable BINd encoder, byte-exact | M | Your own client installation | Round-trips the client's object format back to identical bytes over a 2000-file sample. The encoder is written and decoding is tested; what is missing is that proof at scale and whatever it breaks |
| 16.01 | FileBinary table codec | S | A build; the dev-gated check needs a file you obtained yourself | The patch server's table format, byte for byte, with the first bytes of a written list spelled out in the check |

## Reserved

Everything not in the table above, including every milestone whose dependencies are met but which is listed here, so `apps/progress/ready.py` says so rather than leaving it to be guessed.

| ID | Why |
|---|---|
| 1.18 | Answered against the maintainer's own capture of a session |
| 1.21 | Run against the maintainer's own client and launcher |
| 3.12 | Its remaining checks wait for 6.10 and for a real client session |
| 3.23 | Next in the maintainer's own queue |
| 5.08 | The installer touches packaging and CI, which are the maintainer's |
| 16.11 | Overlaps the type extraction already built in 3.21 and is being rethought |
| 17.01 | One Dev-gated check, on the maintainer's own Windows console and Linux terminal |
| 17.09 | Next in the maintainer's own queue |
| 17.23 | Dev-gated on a reboot and a Pterodactyl install |
| 17.47 | Being built now |
| 17.73 | The panel's design system, being built now |
| 17.106 | Being built now |

## In flight

Nothing yet.

| ID | Who | Pull request | What is left |
|---|---|---|---|

## Landed

Nothing yet.

| ID | Who | Pull request | What landed |
|---|---|---|---|
