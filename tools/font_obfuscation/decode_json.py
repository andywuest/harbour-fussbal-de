#!/usr/bin/env python3
"""Apply a stored fussball.de font mapping to decode an obfuscated JSON file.

Reads the PUA -> plain-text mapping produced by decode_font.py and replaces
every Private Use Area codepoint (U+E000-U+F8FF) in every string of the JSON
payload with the corresponding plain text.

    python3 decode_font.py <fontId> -o mapping.json
    python3 decode_json.py <widget.json> mapping.json -o decoded.json
"""

import argparse
import json
import re

PUA_RE = re.compile(r"[\ue000-\uf8ff]")


def load_mapping(path: str) -> dict:
    with open(path, encoding="utf-8") as fh:
        data = json.load(fh)
    return {int(cp, 16): ch for cp, ch in data["mapping"].items()}


def _decode_string(value: str, mapping: dict) -> str:
    out = []
    for ch in value:
        cp = ord(ch)
        out.append(mapping.get(cp, ch))
    return "".join(out)


def decode(value, mapping: dict):
    if isinstance(value, dict):
        return {k: decode(v, mapping) for k, v in value.items()}
    if isinstance(value, list):
        return [decode(v, mapping) for v in value]
    if isinstance(value, str) and PUA_RE.search(value):
        return _decode_string(value, mapping)
    return value


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("json_file", help="the obfuscated JSON file to decode")
    ap.add_argument("mapping_file", help="mapping JSON written by decode_font.py")
    ap.add_argument("-o", "--output", help="output file (default: stdout)")
    args = ap.parse_args()

    with open(args.json_file, encoding="utf-8") as fh:
        payload = json.load(fh)
    mapping = load_mapping(args.mapping_file)
    result = decode(payload, mapping)

    text = json.dumps(result, ensure_ascii=False, indent=2)
    if args.output:
        with open(args.output, "w", encoding="utf-8") as fh:
            fh.write(text + "\n")
    else:
        print(text)


if __name__ == "__main__":
    main()