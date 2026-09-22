<!-- Project Ambrose by Imjustchico: JSON schemas, fixtures and compatibility checks for contributor-track API contracts. -->

# C-57: Apps and capabilities schemas

This directory contains the version-one JSON contract for `GET /api/apps` and
`GET /api/capabilities`, copied from the shipped encoder in
`src/server/shared/Admin/AdminStatus.cpp` and the admin API description in
`doc/PANEL.md`.

`GET /api/apps` returns an array with one object per app. Each object holds:

- `name`, `role`, `realm`, `address`, `port`, `revision`

`GET /api/capabilities` returns an object with:

- `schema`
- `reload_targets`
- `schedule_actions`
- `announcement_channels`
- `problem_codes`, where each item is `{ "code": string, "description": string }`

Both v2 files are additive compatibility examples. A checker refuses a v2 file
that drops or renames a v1 property.

Run from the repository root:

```powershell
python contrib\schemas\check_c57_schemas.py
```

The fixtures are synthetic values built from the encoder contract. No live
capture or client-derived data is claimed here.
