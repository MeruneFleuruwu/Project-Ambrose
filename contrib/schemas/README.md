<!-- Project Ambrose by Imjustchico: JSON schemas, fixtures and compatibility checks for contributor-track API contracts. -->

# Admin API schemas

This directory holds the version-one JSON contracts for the admin API routes that milestone 17.03 shipped. Each one is copied from the encoder in `src/server/shared/Admin/AdminStatus.cpp` and the "Status API" section of `doc/PANEL.md`. Each has one fixture per app, a v2 file that shows an additive change, and a standard-library checker that validates the fixtures and refuses a v2 which drops or renames a v1 property.

## C-56: `GET /api/status`

The status response has these top-level fields:

- `schema`, `app`, `role`, `realm`, `revision`, `state`: strings except for
  the integer `schema`;
- `uptime`: integer seconds;
- `memory`: an object containing integer `resident_bytes`, or `null`;
- `threads` and `sessions`: integers or `null`;
- `tick`: an object containing `average_ms`, `max_ms`, `samples` and
  `window_seconds`, or `null`;
- `stats`: an object whose values are published by running subsystems; and
- `problems`: an array of `{code, message, subject}` objects.

`address` and `port` are not status fields; they belong to `GET /api/apps`.

```powershell
python contrib\schemas\check_status_schema.py
```

`status-gameserver.json` is a live answer: the maintainer started a gameserver
with `Admin.Enable = 1` and no client install, and saved what `GET /api/status`
returned, which is why it carries the `install_missing` problem and a null
`sessions`. `status-loginserver.json` is still a shape-faithful synthetic
response derived from the encoder.

## C-57: `GET /api/apps` and `GET /api/capabilities`

`GET /api/apps` returns an array with one object per app, each holding `name`,
`role`, `realm`, `address`, `port` and `revision`.

`GET /api/capabilities` returns an object with:

- `schema`
- `reload_targets`
- `schedule_actions`
- `announcement_channels`
- `problem_codes`, where each item is `{ "code": string, "description": string }`

```powershell
python contrib\schemas\check_c57_schemas.py
```

The fixtures are synthetic values built from the encoder contract.
