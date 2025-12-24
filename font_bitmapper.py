#!/usr/bin/env python3
import os
import json
import math
import re
from dataclasses import dataclass
from typing import Dict, List, Tuple

from PIL import Image
import freetype


FONTS_DIR = "ressources/fonts"
OUT_DIR = "generated_fonts"

SIZES_PX = [12, 16, 20, 24, 32, 48]

PADDING_PX = 2
ATLAS_FORCE_SIZE = 0
ATLAS_POW2 = True
INCLUDE_KERNING = False
CHARSET = "".join(chr(i) for i in range(32, 0x2ff))  # Unicode BMP



# --------------------
# Internal
# --------------------
@dataclass
class Glyph:
    codepoint: int
    char: str
    w: int
    h: int
    bearing_x: int
    bearing_y: int
    advance: int
    x: int = 0
    y: int = 0


def sanitize_name(name: str) -> str:
    name = os.path.splitext(os.path.basename(name))[0]
    name = re.sub(r"[^a-zA-Z0-9_\-\.]+", "_", name)
    return name.strip("_")


def next_pow2(x: int) -> int:
    return 1 if x <= 1 else 2 ** (x - 1).bit_length()


def shelf_pack(glyphs: List[Glyph], atlas_w: int, atlas_h: int, padding: int) -> bool:
    x = padding
    y = padding
    row_h = 0

    for g in glyphs:
        gw = g.w + padding
        gh = g.h + padding

        if gw > atlas_w or gh > atlas_h:
            return False

        if x + gw > atlas_w:
            x = padding
            y += row_h + padding
            row_h = 0

        if y + gh > atlas_h:
            return False

        g.x = x
        g.y = y

        x += gw
        row_h = max(row_h, g.h)

    return True


def build_bitmap_font(
    font_path: str,
    px_size: int,
    chars: str,
    padding: int,
    atlas_force: int,
    pow2: bool,
    include_kerning: bool,
) -> Tuple[Image.Image, Dict]:
    face = freetype.Face(font_path)
    face.set_pixel_sizes(0, px_size)

    ascent = face.size.ascender >> 6
    descent = face.size.descender >> 6  # souvent négatif
    line_height = face.size.height >> 6

    glyphs: List[Glyph] = []
    for ch in chars:
        face.load_char(ch, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_NORMAL)
        slot = face.glyph
        bmp = slot.bitmap

        w = bmp.width
        h = bmp.rows
        bearing_x = slot.bitmap_left
        bearing_y = slot.bitmap_top
        advance = slot.advance.x >> 6

        glyphs.append(Glyph(ord(ch), ch, w, h, bearing_x, bearing_y, advance))

    # Atlas size
    if atlas_force > 0:
        atlas_w = atlas_h = atlas_force
        if pow2:
            atlas_w = atlas_h = next_pow2(atlas_force)
    else:
        total_area = sum((g.w + padding) * (g.h + padding) for g in glyphs)
        side = int(math.sqrt(total_area) * 1.25) + 64
        side = max(side, 256)
        atlas_w = atlas_h = next_pow2(side) if pow2 else side

    # Pack (grow if needed)
    while True:
        if shelf_pack(glyphs, atlas_w, atlas_h, padding):
            break
        atlas_w *= 2
        atlas_h *= 2
        if atlas_w > 8192 or atlas_h > 8192:
            raise RuntimeError(f"Atlas too large for {font_path} size={px_size}. Reduce charset or size.")

    # Alpha atlas
    atlas = Image.new("L", (atlas_w, atlas_h), 0)

    # Raster + paste
    for g in glyphs:
        if g.w == 0 or g.h == 0:
            continue
        face.load_char(g.char, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_NORMAL)
        bmp = face.glyph.bitmap
        glyph_img = Image.frombytes("L", (bmp.width, bmp.rows), bytes(bmp.buffer))
        atlas.paste(glyph_img, (g.x, g.y))

    meta = {
        "font": {
            "source": os.path.basename(font_path),
            "px_size": px_size,
            "ascent": ascent,
            "descent": descent,
            "line_height": line_height,
            "atlas_w": atlas_w,
            "atlas_h": atlas_h,
            "padding": padding,
        },
        "glyphs": {},
        "kerning": {},
    }

    for g in glyphs:
        u0 = g.x / atlas_w
        v0 = g.y / atlas_h
        u1 = (g.x + g.w) / atlas_w
        v1 = (g.y + g.h) / atlas_h

        meta["glyphs"][str(g.codepoint)] = {
            "char": g.char,
            "x": g.x, "y": g.y, "w": g.w, "h": g.h,
            "bearing_x": g.bearing_x,
            "bearing_y": g.bearing_y,   # top depuis la baseline
            "advance": g.advance,
            "u0": u0, "v0": v0, "u1": u1, "v1": v1,
        }

    if include_kerning:
        cps = [ord(c) for c in chars]
        for cp1 in cps:
            for cp2 in cps:
                k = face.get_kerning(cp1, cp2, freetype.FT_KERNING_DEFAULT).x >> 6
                if k != 0:
                    meta["kerning"][f"{cp1},{cp2}"] = k

    return atlas, meta


def find_font_files(root: str) -> List[str]:
    exts = {".ttf", ".otf", ".ttc"}
    out = []
    for dirpath, _, filenames in os.walk(root):
        for fn in filenames:
            if os.path.splitext(fn)[1].lower() in exts:
                out.append(os.path.join(dirpath, fn))
    return sorted(out)


def main():
    os.makedirs(OUT_DIR, exist_ok=True)

    font_files = find_font_files(FONTS_DIR)
    if not font_files:
        print(f"[!] Aucun .ttf/.otf trouvé dans {FONTS_DIR}/")
        return

    manifest = {"fonts": []}

    for font_path in font_files:
        font_name = sanitize_name(font_path)
        print(f"\n== Font: {font_name} ({font_path})")

        font_entry = {"name": font_name, "source": font_path, "sizes": []}

        for size in SIZES_PX:
            out_subdir = os.path.join(OUT_DIR, font_name, f"{size}px")
            os.makedirs(out_subdir, exist_ok=True)

            out_png = os.path.join(out_subdir, "atlas.png")
            out_json = os.path.join(out_subdir, "font.json")

            atlas, meta = build_bitmap_font(
                font_path=font_path,
                px_size=size,
                chars=CHARSET,
                padding=PADDING_PX,
                atlas_force=ATLAS_FORCE_SIZE,
                pow2=ATLAS_POW2,
                include_kerning=INCLUDE_KERNING,
            )

            atlas.save(out_png)
            with open(out_json, "w", encoding="utf-8") as f:
                json.dump(meta, f, ensure_ascii=False, indent=2)

            print(f"  - {size}px -> {out_png} ({atlas.size[0]}x{atlas.size[1]})")

            font_entry["sizes"].append({
                "px": size,
                "atlas_png": out_png.replace("\\", "/"),
                "meta_json": out_json.replace("\\", "/"),
                "atlas_w": meta["font"]["atlas_w"],
                "atlas_h": meta["font"]["atlas_h"],
            })

        manifest["fonts"].append(font_entry)

    manifest_path = os.path.join(OUT_DIR, "fonts_manifest.json")
    with open(manifest_path, "w", encoding="utf-8") as f:
        json.dump(manifest, f, ensure_ascii=False, indent=2)

    print(f"\n[OK] Manifest: {manifest_path}")


if __name__ == "__main__":
    main()
