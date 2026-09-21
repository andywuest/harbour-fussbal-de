#!/usr/bin/env python3
"""Decode the fussball.de obfuscated font and store the resulting mapping.

The Next.js widget API (e.g. .../widget/competition/<uuid>/spieltag/<n>.json)
escapes user-visible text into Private Use Area codepoints (U+E000-U+F8FF).
Each payload carries the id of its per-payload obfuscated font in
pageProps.obfuscatedFont, served from:

    https://www.fussball.de/export.fontface/-/format/woff/id/<fontId>/type/font

In that font every PUA codepoint maps (via the 'cmap' table) to a glyph whose
Adobe Glyph List name is the actual character. This script builds the
PUA-codepoint -> plain-text mapping and stores it in a JSON file (the mapping
is per payload, so use the id from the payload itself).

The font id rotates per payload; fetch it from the payload first:

    python3 decode_font.py <fontId> -o mapping.json
"""

import argparse
import io
import json
import sys
import urllib.request

from fontTools import agl
from fontTools.ttLib import TTFont

FONT_URL = "https://www.fussball.de/export.fontface/-/format/woff/id/{font_id}/type/font"
PUA_MIN, PUA_MAX = 0xE000, 0xF8FF


def _glyph_to_char(glyph: str) -> str:
    try:
        u = agl.toUnicode(glyph)
        if u:
            return u
    except agl.AGLError:
        pass
    if glyph.startswith("uni") and len(glyph) == 7:
        try:
            return chr(int(glyph[3:], 16))
        except ValueError:
            pass
    return glyph


def fetch_font(font_id: str) -> TTFont:
    url = FONT_URL.format(font_id=font_id)
    data = urllib.request.urlopen(url).read()
    return TTFont(io.BytesIO(data))


def font_to_mapping(font: TTFont) -> dict:
    cmap = font.getBestCmap()
    mapping = {}
    for cp, glyph in cmap.items():
        if PUA_MIN <= cp <= PUA_MAX:
            mapping[cp] = _glyph_to_char(glyph)
    return mapping


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("font_id", help="obfuscated font id from pageProps.obfuscatedFont")
    ap.add_argument(
        "-o",
        "--output",
        default="font_mapping.json",
        help="where to store the PUA -> plain-text mapping (default: font_mapping.json)",
    )
    ap.add_argument(
        "--font-file",
        help="use a local woff file instead of downloading (takes precedence over font_id)",
    )
    args = ap.parse_args()

    if args.font_file:
        font = TTFont(args.font_file)
    else:
        font = fetch_font(args.font_id)

    mapping = font_to_mapping(font)
    payload = {
        "fontId": args.font_id if not args.font_file else None,
        "mapping": {f"{cp:04x}": ch for cp, ch in sorted(mapping.items())},
    }
    with open(args.output, "w", encoding="utf-8") as fh:
        json.dump(payload, fh, ensure_ascii=False, indent=2)
    print(f"wrote {len(mapping)} PUA mappings to {args.output}")


if __name__ == "__main__":
    main()