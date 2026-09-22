# Project Ambrose by Imjustchico
# Validates the C-59 synthetic metric-series fixture against the dashboard ring contract.

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parent
FILE = ROOT / "c59-metric-series.json"

REQUIRED_SERIES = {"tick_average_ms", "tick_max_ms", "sessions", "memory_resident_bytes"}


def fail(message: str) -> None:
    raise ValueError(message)


def is_number(value: Any) -> bool:
    return isinstance(value, (int, float)) and not isinstance(value, bool)


def validate_series(name: str, series: Any) -> None:
    if not isinstance(series, dict):
        fail(f"{name}: expected a dict")
    times = series.get("times")
    values = series.get("values")
    if not isinstance(times, list) or not isinstance(values, list):
        fail(f"{name}: reuses a time/value pair")
    if len(times) != 60 or len(values) != 60:
        fail(f"{name}: expected 60 points, got {len(times)} times and {len(values)} values")
    last = None
    for index, timestamp in enumerate(times):
        if not is_number(timestamp):
            fail(f"{name}: time at index {index} is not numeric")
        if last is not None and timestamp <= last:
            fail(f"{name}: times are not strictly increasing at index {index}")
        last = timestamp
        value = values[index]
        if value is None:
            continue
        if not is_number(value):
            fail(f"{name}: value at index {index} is not numeric or null")
        if name == "sessions" and value < 0:
            fail(f"{name}: sessions must be non-negative")
        if name == "memory_resident_bytes" and value <= 0:
            fail(f"{name}: memory resident bytes must be positive")
    null_run = 0
    seen_gap = False
    restart_seen = False
    for index, value in enumerate(values):
        if value is None:
            null_run += 1
            continue
        if null_run > 0:
            seen_gap = True
            if index > 0 and index < len(values):
                restart_seen = True
            null_run = 0
    if not seen_gap:
        fail(f"{name}: expected one gap created by null values")
    if name in {"tick_average_ms", "tick_max_ms", "sessions", "memory_resident_bytes"}:
        before_gap = [v for v in values if v is not None]
        if not before_gap:
            fail(f"{name}: no non-null values before the restart")
        if not any(value is None for value in values):
            fail(f"{name}: the fixture must include a null gap")
    if not restart_seen:
        fail(f"{name}: expected a restart after the gap")


def main() -> int:
    try:
        payload = json.loads(FILE.read_text(encoding="utf-8"))
        if not isinstance(payload, dict) or payload.get("version") != 1:
            fail("root must be a version-1 object")
        if payload.get("window_seconds") != 60:
            fail("window_seconds must be 60")
        if payload.get("interval_seconds") != 1:
            fail("interval_seconds must be 1")
        series = payload.get("series")
        if not isinstance(series, dict):
            fail("series must be a dict of metric rings")
        if set(series) != REQUIRED_SERIES:
            fail(f"series keys mismatch: expected {sorted(REQUIRED_SERIES)}")
        for name in sorted(REQUIRED_SERIES):
            validate_series(name, series[name])
        print(f"C-59 metric series valid: {len(REQUIRED_SERIES)} rings over {payload['window_seconds']} seconds")
        return 0
    except (OSError, UnicodeError, json.JSONDecodeError, ValueError) as exc:
        print(f"C-59 metric series: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
