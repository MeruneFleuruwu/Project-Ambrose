<!-- Project Ambrose by Imjustchico: JSON schemas, fixtures and compatibility checks for contributor-track API contracts. -->

# C-56: Status API schema

This directory contains the version-one `GET /api/status` contract from
milestone 17.03, copied from `src/server/shared/Admin/AdminStatus.cpp` and the
status API review in `doc/PANEL.md`.

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
The v2 schema is an additive compatibility example and the checker refuses
removed or renamed v1 properties.

Run from the repository root:

```powershell
python contrib\schemas\check_status_schema.py
```

`status-gameserver.json` is a live answer: the maintainer started a gameserver
with `Admin.Enable = 1` and no client install, and saved what `GET /api/status`
returned, which is why it carries the `install_missing` problem and a null
`sessions`. `status-loginserver.json` is still a shape-faithful synthetic
response derived from the encoder.
