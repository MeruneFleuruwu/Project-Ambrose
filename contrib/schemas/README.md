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

The fixtures are shape-faithful synthetic responses derived from the encoder
and review data. A live server response could not be saved because this
workspace has no configured vcpkg toolchain or built server; the checker does
not claim that these fixtures are live captures.
