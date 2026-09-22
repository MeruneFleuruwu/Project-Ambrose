# Project Ambrose by Imjustchico
# Checks C-57 apps and capabilities fixtures and additive schema compatibility.

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).parent


def load(name: str) -> dict[str, Any] | list[Any]:
    with (ROOT / name).open(encoding="utf-8") as stream:
        return json.load(stream)


def check_type(value: Any, definition: dict[str, Any], path: str) -> None:
    expected = definition.get("type")
    valid = {
        "object": isinstance(value, dict),
        "array": isinstance(value, list),
        "string": isinstance(value, str),
        "integer": isinstance(value, int) and not isinstance(value, bool),
    }
    if expected in valid and not valid[expected]:
        raise ValueError(f"{path}: expected {expected}")
    if isinstance(value, (int, float)) and not isinstance(value, bool):
        if "minimum" in definition and value < definition["minimum"]:
            raise ValueError(f"{path}: below minimum")
    if isinstance(value, str) and "minLength" in definition and len(value) < definition["minLength"]:
        raise ValueError(f"{path}: shorter than minLength")


def resolve(definition: dict[str, Any], root: dict[str, Any]) -> dict[str, Any]:
    reference = definition.get("$ref")
    if not reference:
        return definition
    if not reference.startswith("#/$defs/"):
        raise ValueError(f"unsupported schema reference: {reference}")
    return root["$defs"][reference.removeprefix("#/$defs/")]


def validate(value: Any, definition: dict[str, Any], root: dict[str, Any], path: str = "$") -> None:
    definition = resolve(definition, root)
    check_type(value, definition, path)
    if isinstance(value, dict):
        required = definition.get("required", [])
        missing = [name for name in required if name not in value]
        if missing:
            raise ValueError(f"{path}: missing required fields {', '.join(missing)}")
        properties = definition.get("properties", {})
        if definition.get("additionalProperties") is False:
            unknown = [name for name in value if name not in properties]
            if unknown:
                raise ValueError(f"{path}: unknown fields {', '.join(unknown)}")
        for name, child in value.items():
            if name in properties:
                validate(child, properties[name], root, f"{path}.{name}")
    elif isinstance(value, list) and "items" in definition:
        for index, child in enumerate(value):
            validate(child, definition["items"], root, f"{path}[{index}]")


def property_paths(definition: dict[str, Any], root: dict[str, Any], path: str = "$") -> set[tuple[str, str]]:
    definition = resolve(definition, root)
    names = {(path, name) for name in definition.get("properties", {})}
    for name, child in definition.get("properties", {}).items():
        names.update(property_paths(child, root, f"{path}.{name}"))
    if "items" in definition:
        names.update(property_paths(definition["items"], root, f"{path}[]"))
    return names


def required_paths(definition: dict[str, Any], root: dict[str, Any], path: str = "$") -> set[tuple[str, str]]:
    definition = resolve(definition, root)
    names = {(path, name) for name in definition.get("required", [])}
    for name, child in definition.get("properties", {}).items():
        names.update(required_paths(child, root, f"{path}.{name}"))
    if "items" in definition:
        names.update(required_paths(definition["items"], root, f"{path}[]"))
    return names


def check_compatibility(v1: dict[str, Any], v2: dict[str, Any], label: str) -> None:
    removed_required = required_paths(v1, v1) - required_paths(v2, v2)
    removed_properties = property_paths(v1, v1) - property_paths(v2, v2)
    if removed_required:
        raise ValueError(f"{label} removed required fields: {sorted(removed_required)}")
    if removed_properties:
        raise ValueError(f"{label} removed properties: {sorted(removed_properties)}")


def main() -> None:
    apps_v1 = load("apps-v1.json")
    capabilities_v1 = load("capabilities-v1.json")
    for fixture_name in ("apps-gameserver.json", "apps-loginserver.json"):
        validate(load(fixture_name), apps_v1, apps_v1, fixture_name)
    for fixture_name in ("capabilities-gameserver.json", "capabilities-loginserver.json"):
        validate(load(fixture_name), capabilities_v1, capabilities_v1, fixture_name)
    check_compatibility(apps_v1, load("apps-v2.json"), "apps-v2")
    check_compatibility(capabilities_v1, load("capabilities-v2.json"), "capabilities-v2")
    print("validated 4 C-57 fixtures; v2 schemas preserve all v1 properties")


if __name__ == "__main__":
    main()
