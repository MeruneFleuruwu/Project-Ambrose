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
