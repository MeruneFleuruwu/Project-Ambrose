<!-- Project Ambrose by Imjustchico: Synthetic fixture corpora for contributor-track parsers and protocol-independent tests. -->

# Contributor fixture corpora

The C-58 corpus is a hand-shaped, deterministic set of 500 synthetic Ambrose
console records. It contains no captures, client-derived text, credentials,
real account names, or private paths.

Run its validator from the repository root:

```powershell
python contrib\fixtures\validate_c58.py
```

The validator checks record identity, fixed-column level and category fields,
all six severity levels, the documented logging categories, every expected
C-29 value-class span, UTF-8 byte offsets, and the required C-58 count.

The C-59 fixture is a deterministic sixty-second metric ring for the dashboard's
live status view. It contains a two-second gap where samples were missed but the process kept running, and a five-second gap followed by a restart, listed in `restarts` by the time the process came back, so the chart can show a
process stop and a clean relaunch without inventing a bogus zero-value sample.

Run its validator from the repository root:

```powershell
python contrib\fixtures\validate_c59.py
```

The validator checks the ring contract: exactly sixty seconds, a pair of
`times` and `values` arrays for each metric, the null gap and restart, and the
expected session, tick and resident-memory ranges.
