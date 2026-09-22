<!-- Project Ambrose by Imjustchico: C-57 schemas, fixtures and compatibility checks for the apps and capabilities API contracts. -->

# C-57: Apps and capabilities API schemas

This directory's C-57 files describe the version-one contracts for
`GET /api/apps` and `GET /api/capabilities` from roadmap milestone 17.03.
The schemas allow later additive fields while requiring the fields the
milestone promises every app or build can answer.

The fixtures are synthetic, one for each current app, and contain no
credentials, captures, client-derived data or private paths. The checker
validates every fixture and compares the version-one schemas with their
version-two compatibility examples, refusing removed or renamed properties.

Run from the repository root:

```powershell
python contrib\schemas\check_c57_schemas.py
```
