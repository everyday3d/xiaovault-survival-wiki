#!/usr/bin/env python3
"""
XiaoVault Survival Wiki — Micro-Wiki SD Card Builder
Converts a directory of Markdown / HTML files or text articles into a high-speed
binary-indexed file format (`wiki.idx` and `wiki.dat`) optimized for ESP32 microcontrollers.

Index Format:
Header (8 bytes):
  - [4 bytes] Magic: 0x57494B49 ("WIKI")
  - [4 bytes] Total Entry Count (uint32_t)

Per Entry (44 bytes, sorted alphabetically by title):
  - [4 bytes]  FNV-1a Hash of title (uint32_t)
  - [28 bytes] Title string (ASCII, null-padded)
  - [8 bytes]  Data file offset (uint64_t)
  - [4 bytes]  Article byte length (uint32_t)

Usage:
  python build_wiki_sd.py --input ./sample_articles --output ./sd_card_root/wiki
"""

import os
import sys
import struct
import argparse
from pathlib import Path

# Ensure UTF-8 output on Windows consoles
if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8')

MWIKI_MAGIC = 0x57494B49 # "WIKI"
ENTRY_SIZE = 44
TITLE_MAX_LEN = 28

def fnv1a_32(text: str) -> int:
    h = 2166136261
    for byte in text.lower().encode('utf-8', errors='ignore'):
        h ^= byte
        h = (h * 16777619) & 0xFFFFFFFF
    return h

def build_wiki(input_dir: Path, output_dir: Path):
    output_dir.mkdir(parents=True, exist_ok=True)
    idx_path = output_dir / "wiki.idx"
    dat_path = output_dir / "wiki.dat"

    print(f"📖 Scanning articles in: {input_dir}")
    articles = []

    for file_path in input_dir.rglob("*"):
        if file_path.is_file() and file_path.suffix.lower() in [".md", ".html", ".htm", ".txt"]:
            title = file_path.stem.replace("_", " ").replace("-", " ").title()
            try:
                with open(file_path, "r", encoding="utf-8", errors="replace") as f:
                    content = f.read()
                
                # Wrap markdown in simple HTML if needed
                if file_path.suffix.lower() == ".md":
                    html_content = f"<div><pre style='white-space: pre-wrap; font-family: sans-serif;'>{content}</pre></div>"
                else:
                    html_content = content

                content_bytes = html_content.encode("utf-8")
                articles.append({
                    "title": title[:TITLE_MAX_LEN],
                    "hash": fnv1a_32(title[:TITLE_MAX_LEN]),
                    "bytes": content_bytes
                })
            except Exception as e:
                print(f"⚠️ Error reading {file_path}: {e}")

    # Sort articles alphabetically by title (CRITICAL for Binary Search on ESP32!)
    articles.sort(key=lambda a: a["title"].lower())
    total_count = len(articles)
    print(f"✅ Found and sorted {total_count} articles.")

    print(f"💾 Writing {dat_path} and {idx_path}...")
    with open(dat_path, "wb") as f_dat, open(idx_path, "wb") as f_idx:
        # Write index header (8 bytes): magic + count
        f_idx.write(struct.pack("<II", MWIKI_MAGIC, total_count))

        current_offset = 0
        for art in articles:
            length = len(art["bytes"])
            f_dat.write(art["bytes"])

            # Format title as 28-byte null-padded ASCII
            title_bytes = art["title"].encode("ascii", errors="replace")[:TITLE_MAX_LEN]
            title_padded = title_bytes.ljust(TITLE_MAX_LEN, b"\x00")

            # Entry (44 bytes): [hash: 4][title: 28][offset: 8][length: 4]
            entry = struct.pack("<I28sQI", art["hash"], title_padded, current_offset, length)
            f_idx.write(entry)

            current_offset += length

    print(f"🎉 Build Complete!")
    print(f"   Indexed: {total_count} articles")
    print(f"   Data Size:  {current_offset / 1024:.1f} KB")
    print(f"   Index Size: {os.path.getsize(idx_path) / 1024:.1f} KB")
    print(f"👉 Copy the '{output_dir.name}' folder onto the root of your FAT32 MicroSD card!")

def create_sample_pack(sample_dir: Path):
    sample_dir.mkdir(parents=True, exist_ok=True)
    samples = {
        "Edible_Wild_Plants.md": """# Edible Wild Plants Guide
## Universal Edibility Test (Extreme Emergencies Only)
1. Separate plant into parts (roots, stems, leaves, flowers).
2. Test one part at a time.
3. Crush and smell: Discard if smelling like bitter almonds (cyanide).
4. Skin contact test: Rub on wrist/elbow for 15 minutes. Wait 8 hours for rash.
5. Lip contact test: Touch to outer lip for 3 minutes.
6. Tongue test: Place on tongue for 15 minutes without chewing/swallowing.
7. Small bite: Chew small portion and hold 15 minutes.
8. Wait 8 hours. If no nausea, cramping, or diarrhea, plant part is generally safe.

## Reliable Common Edibles (North America & Europe)
- Dandelion: 100% edible (leaves, flowers, roots roasted for tea).
- Cattails: "Supermarket of the swamp". White stem core, pollen, root starch.
- Plantain (Plantago major): Young leaves raw or boiled; crushed leaves soothe bee stings.
- Pine Needles: Rich in Vitamin C (brew as tea; avoid Ponderosa pine).
""",
        "Ham_Radio_Frequencies.md": """# Emergency Radio Frequencies
## National Calling Frequencies (VHF / UHF)
- 146.520 MHz (2 Meter National FM Simplex Calling)
- 446.000 MHz (70 Centimeter National FM Simplex Calling)
- 52.525 MHz (6 Meter FM Calling)

## Marine VHF Emergency
- Channel 16 (156.800 MHz): International Maritime Distress & Hailing.

## CB Radio (Citizens Band)
- Channel 9 (27.065 MHz): Emergency & Roadside Assistance.
- Channel 19 (27.185 MHz): Highway & Trucker information.

## NOAA Weather Radio Frequencies
- WX1: 162.550 MHz | WX2: 162.400 MHz | WX3: 162.475 MHz
- WX4: 162.425 MHz | WX5: 162.450 MHz | WX6: 162.500 MHz | WX7: 162.525 MHz
""",
        "Essential_Knots.md": """# Essential Survival Knots
1. Bowline (King of Knots): Creates a fixed, secure loop that never slips or jams under heavy tension. Essential for rescue harnesses.
2. Square Knot (Reef Knot): Quick binding knot for tying two ropes of equal diameter together (bandages, bundles).
3. Taut-Line Hitch: Adjustable friction hitch for tightening tent guylines and tarp ridges.
4. Clove Hitch: Quickly secures a rope to a post, tree, or ridge pole.
5. Figure-Eight on a Bight: Reliable stopper knot and climbing anchor loop.
"""
    }

    for name, text in samples.items():
        with open(sample_dir / name, "w", encoding="utf-8") as f:
            f.write(text.strip())

    print(f"📦 Created sample survival guides in: {sample_dir}")

def main():
    parser = argparse.ArgumentParser(description="Build indexed offline wiki for XiaoVault ESP32")
    parser.add_argument("--input", default="sample_articles", help="Input directory of markdown/html articles")
    parser.add_argument("--output", default="sd_card/wiki", help="Output directory for wiki.idx and wiki.dat")
    parser.add_argument("--create-samples", action="store_true", help="Generate sample survival articles")
    args = parser.parse_args()

    input_p = Path(args.input)
    output_p = Path(args.output)

    if args.create_samples or not input_p.exists():
        create_sample_pack(input_p)

    build_wiki(input_p, output_p)

if __name__ == "__main__":
    main()
