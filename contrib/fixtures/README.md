<!-- Project Ambrose by Imjustchico: Synthetic console-log corpus for the value classifier and the console's own layout rules. -->

# C-58: log corpus

This directory holds a deterministic corpus of 500 synthetic console log lines.
The file is shaped to the console's fixed-column layout and to the 13 logging
categories documented in `doc/guides/logging.md`:

- `server.app`
- `server.config`
- `server.logging`
- `network`
- `network.session`
- `server.admin`
- `server.threading`
- `server.loading`
- `sql.updates`
- `sql.driver`
- `network.opcode`
- `sql`
- `sql.sql`

Each record carries:

- a stable `id`;
- the full `line` text;
- the `level` and `category` metadata;
- the expected `C-29` value spans with `value_class`, `text`, `start` and `end`
  using UTF-8 byte offsets.

The validator imports the classifier in
`contrib/tools/ambrose-log-value-classifier/classify.py`, checks that each saved
span matches what the classifier emits, and requires all six levels and all 13
categories to be present.

Run it from the repository root:

```powershell
python contrib\fixtures\validate_c58.py
```

The corpus is synthetic only. It contains no captures, no client-derived data,
and no private paths.
