<!-- Project Ambrose by Imjustchico: How a contributor pull request is reviewed here, as the rules, the checklist and the lessons that produced them, so a review after a lost context starts from the same place. -->

# Reviewing contributor pull requests

This is the maintainer's side of doc/CONTRIBUTOR-TRACK.md. It says how a pull request on the track is judged, in what order, and what has gone wrong before. Read it whole before reviewing anything, and when it changes, change it here rather than in a review comment.

## The rules that do not bend

- **Verify by running, never by reading.** A tool is built and run, on its own self-test and on a real input. A scenario is loaded through the driver's own validator and every log pattern it waits on is found in the server source. A guide's every backticked path and option is checked against the tree. A proposal's premise is checked against `doc/ROADMAP.md`'s "Where we are", the phase file for its area and the tests, because two proposals in one day set out to add what already existed. A plausible summary is not evidence.
- **A finding states what the game does.** A record that the repository holds no evidence is closed unmerged, and `apps/ci/ci_findings.py` refuses the shape by name. Forty-one arrived in one batch.
- **An item is done when its row moves to the merged table, and not before.** The shape of an item is not the item: a table with no rows, a README about seeds, a catalog of scenarios someone else wrote. Merge the work if it is sound, keep the item open, and record the partial delivery under "Started, still open".
- **One message per pull request, at merge or close time, carrying everything.** What was verified and how, what was wrong and why, what is being fixed on main instead of sent back, and what to pick up next. No running commentary, no separate follow-up.
- **Fix on main, credit the contributor.** The pre-push hook blocks every remote but the maintainer's, so a fix never goes to a fork: merge, then commit the fix on main, naming the pull request and keeping the contributor as the merge commit's co-author with their numeric noreply address, `<id>+<login>@users.noreply.github.com`. Look the id up with `gh api users/<login>`.
- **Never lower a bar to clear a queue.** Sixty-three at once is reviewed the same way as one, in bulk where the checks are mechanical and one at a time where they are not.

## The order of work

1. **List and fetch.** `gh pr list --state open --limit 100`, then fetch every head to a local `prN` branch. Group by kind: scenario, tool with code, note, guide, proposal, finding, locale, data.
2. **Sweep mechanically, in one pass.** For each branch: `ci_contrib_paths.py --range origin/main...prN`, codestyle, `ci_forbidden_files.py`, `ci_findings.py`, and independence, `git rev-list --count origin/main..prN` at one or two and `git diff --name-only` showing only its own files. Anything else is chained or out of bounds and is sent back before it is read.
3. **Approve every pending CI run at once**, so they run in parallel: for each branch, the run whose conclusion is `action_required`, then `gh api -X POST .../actions/runs/<id>/approve`. A first-time contributor needs this on every run. A `labeled` event does not start the checks job; close and reopen to trigger a real run. A pull request opened from a fork's `main` has its runs listed under `main`; find it by number rather than branch.
4. **Verify substance by kind.**
   - Tool: configure and build with its own CMakeLists or run its script, run `--self-test` or its validator, then run it on a real input from this repository and read the output for a wrong answer. The value classifier split an IP address into four numbers; the type diff accepted a version it did not know and reported nothing; the install diff hashed every file before deciding whether it cared.
   - Scenario: load through `apps/clientdriver/clientdriver/scenario.py`'s `load`, then grep the server source for every `pattern` and `command` it uses.
   - Guide or note: every backticked path exists in the tree and every `Section.Option` is found under `src`, `apps` or `doc/config`. Walk it if the environment allows: the Linux guide was run on Ubuntu 24.04 rather than trusted.
   - Proposal: find whether the thing proposed already exists before judging the design. `src/test/apps/AppSmokeTest.cmake` alone already proves the servers reach readiness with disposable databases.
   - Finding: the claim is about the game, the evidence is observations, the method matches, and the revision is real. An experiment with before-and-after hashes of the install is the standard.
   - Schema or fixture for a route that exists: compare its property names and JSON types with the encoder that writes the route, `src/server/shared/Admin/AdminStatus.cpp` for status, apps and capabilities, and validate a response shaped like the real one against it. The first two schemas, C-56 and C-57, passed their own checkers and described fields the server never sends.
   - Locale or data: check whether two pull requests write the same file. Merge the larger, fold the rest onto main, close the other as folded.
5. **Merge with a squash**, subject `<item id>: <what landed>`, body saying what it does, `Contributed on the contributor track.`, and the contributor's `Co-Authored-By`. Never build one branch on another to merge them together.
6. **Close what is not a contribution** with the reason, the item that stays open, and what would make it one.
7. **Update the track in the same sitting.** Move each merged row to "Merged so far" with the path and the pull request, add partial deliveries to "Started, still open", and let the README counts follow. `apps/ci/tests/test_ci.py` fails if an id is in both tables, listed twice, counted wrongly in README.md, or points at a path that does not exist. Run it before pushing.
8. **Fix what review found, on main, then push.** One commit per fix, naming the pull request and the fault, with the contributor as co-author when it is their file.
9. **Update the prompt and this file** when a batch teaches something. `contrib/AI-START-HERE.md` is what a contributor's assistant reads; if a fault repeats, it belongs there as a rule with the case that produced it.

## Reviewing a milestone pull request

doc/MILESTONE-TRACK.md is the contributor's side of this. A milestone pull request is the roadmap itself, so it is read harder than a track item, in this order, and anything that fails an early step is answered before the code is read at all.

1. **It is a milestone the project opened.** The id is in "Open now", or in "In flight" against this contributor. One that is reserved is closed with the reason and the table, however good the code is, because something else is being built in those files right now.
2. **The branch is named `milestone/<id>-<short-name>`.** Without it the path check runs in contributor mode and refuses every source file, and the contributor sees a wall of red they cannot fix. Tell them to rename and force-push rather than guessing what they meant.
3. **Paths.** `python apps/ci/ci_contrib_paths.py --range origin/main...prN --branch <their branch>`, which refuses the maintainer's own files and every phase file but the milestone's own.
4. **Scope.** `git diff --stat origin/main...prN` reads as one milestone. A rename, a reformat or an improvement on the way past makes the rest unreviewable and is sent back on its own.
5. **Nothing from the client, and nothing generated.** The forbidden file scan, then `git diff --name-only` read by eye for an extracted file, a dump or a database file, because several of these milestones generate exactly those next to the source.
6. **Build and run it.** Label it `ci:all` so both compilers see it, and build it locally too. Warnings are errors, and MSVC and GCC disagree about what is a warning, so a contributor who could only build on one platform is normal and is not a fault.
7. **Every ticked box, one at a time.** For each `- [x]`, find the evidence in its brackets in the tree, then run it by name, `ctest --preset <preset> -R <test>`, and read what it asserts. A check ticked by a test that does not exist, or by one that asserts nothing, ends the review: say so plainly, keep the rest, and do not go looking for what else might be wrong until that is answered. For the milestone's central claim, break the code deliberately and watch the test fail, because a test that only ever passed proves nothing.
8. **Every unticked box is accounted for** in the description, as a gated check the contributor could not run. An unticked box nobody mentions is the milestone half-built, which merges with the row left in "In flight" and what is left written next to it.
9. **The phase's review notes** that name the milestone are resolved, in the code or in the description.
10. **Merge, then finish it on main.** Squash with subject `<id>: <what landed>`, the contributor's `Co-Authored-By`, and `Contributed on the milestone track.` in the body. Then on main, in one commit: move the row to "Landed" or "In flight", update doc/ROADMAP.md's "Where we are" to say what is now built, and regenerate the card with `python apps/progress/progress.py`, which is why those files are kept off a milestone branch. `python apps/progress/ready.py` then says what the merge opened up.

## What is automated, so it is not done by hand

None of this replaces a review. It removes the steps that were only ever bookkeeping.

- **The board is the live answer, and holds are how it is steered.** `https://justchicoo.github.io/Project-Ambrose/` and its `state.json` are generated from the roadmap, the tracks, `doc/work/holds.json` and the open pull requests, and they rebuild when a claim opens or closes. To close a milestone or a whole phase to outside work, add a hold with a scope of `milestone:<id>` or `phase:<number>`, who holds it and what they are building; to open it again, delete the entry. The path check refuses a branch for anything held, the self-tests refuse a hold nobody owns or one naming a milestone that does not exist, and `apps/site/tests/test_site.py` refuses a milestone that is open and held at once.
- **A pull request sorts itself.** `.github/workflows/triage.yml` labels it `milestone-track` from its branch name, or `contrib` when every path it changes is inside the track, and greets a first-time contributor once with the document their track is described in. It runs this repository's own code against the pull request's metadata, never the contributor's, which is why it may use `pull_request_target` safely. A mixed or unrecognised pull request is left unlabelled on purpose, because that is a decision.
- **A milestone branch builds itself.** `ci_select_legs.py` adds the Linux GCC leg for any branch named `milestone/<id>`, so a milestone pull request compiles without waiting for a label. Fork runs still need approval for a first-time contributor. Add `ci:windows-msvc-x64` or `ci:all` by hand when the change deserves more.
- **The summary cannot be forgotten.** `ci_roadmap_state.py` fails a push or pull request that ticks acceptance checks without updating doc/ROADMAP.md's "Where we are" in the same change. A milestone branch is exempt, because the roadmap is a file it may not touch, which leaves that update where it belongs, in the commit that finishes the merge. core-build now also runs on a push to main that touches the roadmap, so the progress card cannot go stale there either.
- **Discord keeps two messages, never a thread of them.** `progress.yml` edits the progress card when the generated numbers change, and `openings.yml` edits the list of open milestones when doc/MILESTONE-TRACK.md changes. Each remembers its message id on its own state branch, `progress-state` and `openings-state`, and falls back to the id committed on main. Neither posts anything when the repository has no webhook secret.
- **Dependabot proposes one grouped update a month** for the workflow actions and for the front-end workspace, and never a major version, which is a decision rather than an update.
- **The relay reports more than pull requests.** The webhook that pushes events to the session now also carries issues, comments, reviews and a pull request whose checks failed, so a contributor stuck on a red check is answered without waiting for the next sweep.

## Traps met so far

- Two loops that both `git checkout` in the same worktree corrupt each other. Build tools in a background loop or read files with `git show prN:path`, never both at once.
- A heredoc through the shell turns `\b` into a backspace byte and `\n` into a newline. Write a regex or a test with the Write tool or build it with `chr(92)`, then grep the file for `chr(8)`.
- `gh`, a Windows binary, cannot read an MSYS `/tmp` path. Pipe with `--input -` or use `$TEMP`.
- Redelivering a merge does not re-run CI on the new base; close and reopen does.
- A contributor cannot label from a fork, so the path check keys off the fork instead; nothing needs the `contrib` label to run.
- After a squash merge the branch holds nothing; the next pull request from it is empty.
- A sound pull request that conflicts with main cannot be merged on GitHub, and a fix cannot go to the fork. Run `git merge --squash prN` onto main, resolve the conflict, and commit with `--author` set to the contributor's numeric noreply address and the maintainer as committer. Then push, and close the pull request naming the commit. #103 landed this way as 1e425be.
- `mergeable` reads `UNKNOWN` for a minute after a burst of merges; wait and retry rather than reaching for `--admin`.

## What the batch of sixty-three taught

Twenty-two were real and merged, one of them the only finding that stated something about the game. Forty-one were gap records and were closed. Three shipped the shape without the substance and were merged with their items left open. Two wrote the same locale file and were folded into one. Every one of those outcomes is now a rule above, and the two that mattered most, a finding must be about the game and an item is done only when its row moves, are also in the prompt so they are caught before a pull request is opened rather than in review.
