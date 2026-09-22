<!-- Project Ambrose by Imjustchico: JSON schemas, fixtures and compatibility checks for contributor-track API contracts. -->

# C-56: Status API schema

This directory contains the version-one `GET /api/status` contract described by
roadmap milestone 17.03, one synthetic response fixture for each current
server app, and a standard-library-only checker.

The schema deliberately permits later additive fields. The compatibility
check compares `status-v1.json` with `status-v2.json` and refuses a required
field or property that v2 removes or renames. The v2 file is a synthetic
forward-compatibility example: it adds `players_online` and `players_limit`
without claiming that the current servers emit them.

Run from the repository root:

```powershell
python contrib\schemas\check_status_schema.py
```

The fixtures are hand-written synthetic values. They contain no credentials,
captures, client-derived data or private paths.
