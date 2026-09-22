# Project Ambrose by Imjustchico
# Validates capture-free C-74 replay manifests against the C-23 replayer shape.

import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent
REQUIRED = {
    "c74-login-replay.json": ("login-success", "MSG_USER_AUTHEN_V3", "MSG_USER_AUTHEN_RSP"),
    "c74-wrong-password-replay.json": ("login-wrong-password", "MSG_USER_AUTHEN_V3", "MSG_USER_AUTHEN_RSP"),
    "c74-keepalive-replay.json": ("keepalive-exchange", "CLIENT_KEEPALIVE", "KEEPALIVE_RESPONSE"),
}


def frame_header(vector, label):
    data = bytes.fromhex(vector)
    if len(data) < 9 or data[:2] != b"\x0d\xf0":
        raise ValueError(f"{label}: not an Ambrose frame")
    declared = int.from_bytes(data[2:4], "little")
    if declared != len(data) - 4:
        raise ValueError(f"{label}: frame length is {declared}, actual {len(data) - 4}")
    return data


def check(path):
    document = json.loads(path.read_text(encoding="utf-8"))
    expected_name, first_message, second_message = REQUIRED[path.name]
    if document.get("version") != 1 or document.get("name") != expected_name:
        raise ValueError(f"{path.name}: invalid version or corpus name")
    if not document.get("source", "").startswith("Ambrose-authored"):
        raise ValueError(f"{path.name}: source claims a client capture")
    sequence = document.get("sequence")
    exchanges = document.get("exchanges")
    if not isinstance(sequence, list) or not isinstance(exchanges, list) or len(exchanges) != 1:
        raise ValueError(f"{path.name}: expected one replay exchange and a sequence")
    if sequence[0].get("message") != first_message or sequence[1].get("message") != second_message:
        raise ValueError(f"{path.name}: message-definition sequence does not match C-74")
    exchange = exchanges[0]
    request = frame_header(exchange["request_hex"], f"{path.name} request")
    response = frame_header(exchange["expected_response_hex"], f"{path.name} response")
    if exchange["response_bytes"] != len(response):
        raise ValueError(f"{path.name}: response_bytes does not match response frame")
    if path.name != "c74-keepalive-replay.json":
        if request[4] != 0 or response[4] != 0:
            raise ValueError(f"{path.name}: login vectors must be DML frames")
        if request[8:10] != bytes((7, 27)) or response[8:10] != bytes((7, 14)):
            raise ValueError(f"{path.name}: service/order does not match definitions")
    else:
        if request[4:6] != bytes((1, 3)) or response[4:6] != bytes((1, 4)):
            raise ValueError(f"{path.name}: control opcode does not match keepalive definitions")


def main():
    try:
        for name in REQUIRED:
            check(ROOT / name)
    except (OSError, ValueError, json.JSONDecodeError) as error:
        print(f"C-74 validation failed: {error}", file=sys.stderr)
        return 1
    print(f"C-74 validation passed: {len(REQUIRED)} capture-free replay manifests")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
