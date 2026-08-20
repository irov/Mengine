#!/usr/bin/env python3

import argparse
import plistlib
import sys
from pathlib import Path


def read_entitlements(path: Path) -> dict:
    with path.open("rb") as stream:
        entitlements = plistlib.load(stream)

    if not isinstance(entitlements, dict):
        raise ValueError(f"entitlements root must be a dictionary: {path}")

    return entitlements


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--base", type=Path)
    parser.add_argument("fragments", nargs="+", type=Path)
    args = parser.parse_args()

    merged = read_entitlements(args.base) if args.base is not None else {}

    for fragment_path in args.fragments:
        fragment = read_entitlements(fragment_path)

        for key, value in fragment.items():
            if key in merged and merged[key] != value:
                raise ValueError(
                    f"conflicting entitlement '{key}' in {fragment_path}: "
                    f"base has {merged[key]!r}, fragment requests {value!r}"
                )

            merged[key] = value

    args.output.parent.mkdir(parents=True, exist_ok=True)

    with args.output.open("wb") as stream:
        plistlib.dump(merged, stream, fmt=plistlib.FMT_XML, sort_keys=True)

    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, plistlib.InvalidFileException, ValueError) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
