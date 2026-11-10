#!/usr/bin/env python3
"""
XiaoVault Survival Wiki - SD card builder (index format v2)

Turns a folder of guides (Markdown with simple front matter, or plain
Markdown / HTML / text files) into the files the firmware reads from the
microSD card:

  wiki/wiki.idx        binary index, sorted by title (case-insensitive)
  wiki/wiki.dat        article bodies as ready-to-show HTML
  wiki/categories.tsv  category list for the home screen (copied through)
  wiki/search.tsv      one line per article, "title<TAB>tags<TAB>summary",
                       in index order, so search also matches keywords
  wiki/img/            images used by the guides (copied through)

Index v2 layout (little endian)
  Header, 16 bytes:  magic "WIKI" 0x57494B49 | count u32 | version u32 (=2) | reserved u32
  Entry,  68 bytes:  title[56] UTF-8, null padded
                     offset u32 | length u32 | category u8 | reserved[3]

Guide front matter (same format as the ESP32 CYD Survival Wiki):

  ---
  title: Burns
  category: firstaid          (an id from categories.tsv)
  tags: burns, scald, blisters
  summary: Cool the burn, cover it, and know when it is serious.
  ---

Usage:
  python tools/build_wiki_sd.py                       # guides/ -> sd_card/wiki/
  python tools/build_wiki_sd.py --input my_guides --output E:/wiki
"""

import argparse
import html
import re
import shutil
import struct
import sys
from pathlib import Path

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")

MAGIC = 0x57494B49  # "WIKI"
VERSION = 2
HEADER_SIZE = 16
ENTRY_SIZE = 68
TITLE_BYTES = 56
NO_CATEGORY = 255


def parse_front_matter(text):
    """Split '--- key: value ---' front matter from the body."""
    meta = {}
    if text.startswith("---"):
        end = text.find("\n---", 3)
        if end != -1:
            for line in text[3:end].splitlines():
                if ":" in line:
                    key, _, value = line.partition(":")
                    meta[key.strip().lower()] = value.strip()
            body = text[end + 4:].lstrip("\r\n")
            return meta, body
    return meta, text


def inline(text, link_map):
    """Inline markdown: escape, bold, italic, images, links."""
    text = html.escape(text, quote=False)

    def image(m):
        alt, src = m.group(1), m.group(2)
        src = src.replace("img/", "/wiki/img/", 1) if src.startswith("img/") else src
        return f'<img src="{src}" alt="{alt}" loading="lazy">'

    text = re.sub(r"!\[([^\]]*)\]\(([^)]+)\)", image, text)

    def link(m):
        label, target = m.group(1), m.group(2)
        if target.endswith(".md"):
            title = link_map.get(Path(target).stem)
            if title:
                return f'<a class="xl" data-id="{html.escape(title)}">{label}</a>'
            return label
        if target.startswith(("http://", "https://")):
            return label  # no internet in the field, keep the words only
        return label

    text = re.sub(r"(?<!!)\[([^\]]+)\]\(([^)]+)\)", link, text)
    text = re.sub(r"\*\*(.+?)\*\*", r"<strong>\1</strong>", text)
    text = re.sub(r"(?<![\*\w])\*([^\*\s][^\*]*?)\*(?![\*\w])", r"<em>\1</em>", text)
    return text


def render(body, link_map):
    """Small markdown subset renderer: ## headings, lists, quotes, paragraphs."""
    out = []
    para = []
    list_kind = None
    quote = []

    def flush_para():
        nonlocal para
        if para:
            out.append("<p>" + "<br>".join(inline(p, link_map) for p in para) + "</p>")
            para = []

    def flush_list():
        nonlocal list_kind
        if list_kind:
            out.append(f"</{list_kind}>")
            list_kind = None

    def flush_quote():
        nonlocal quote
        if quote:
            out.append('<blockquote>' + "<br>".join(inline(q, link_map) for q in quote) + "</blockquote>")
            quote = []

    for raw in body.splitlines():
        line = raw.rstrip()
        stripped = line.strip()
        if not stripped:
            flush_para(); flush_list(); flush_quote()
            continue
        if stripped.startswith("> ") or stripped == ">":
            flush_para(); flush_list()
            quote.append(stripped[1:].strip())
            continue
        flush_quote()
        m = re.match(r"^(#{1,4})\s+(.*)", stripped)
        if m:
            flush_para(); flush_list()
            out.append(f"<h4>{inline(m.group(2), link_map)}</h4>")
            continue
        m = re.match(r"^[-*]\s+(.*)", stripped)
        if m:
            flush_para()
            if list_kind != "ul":
                flush_list(); out.append("<ul>"); list_kind = "ul"
            out.append(f"<li>{inline(m.group(1), link_map)}</li>")
            continue
        m = re.match(r"^\d+[.)]\s+(.*)", stripped)
        if m:
            flush_para()
            if list_kind != "ol":
                flush_list(); out.append("<ol>"); list_kind = "ol"
            out.append(f"<li>{inline(m.group(1), link_map)}</li>")
            continue
        flush_list()
        para.append(stripped)
    flush_para(); flush_list(); flush_quote()
    return "\n".join(out)


def load_categories(path):
    ids, names = [], {}
    if path.exists():
        for line in path.read_text(encoding="utf-8").splitlines():
            if line and not line.startswith("#"):
                cols = line.split("\t")
                ids.append(cols[0])
                names[cols[0]] = cols[1] if len(cols) > 1 else cols[0]
    return ids, names


def clip_title(title):
    raw = title.encode("utf-8")
    if len(raw) <= TITLE_BYTES:
        return raw
    return raw[:TITLE_BYTES].decode("utf-8", errors="ignore").encode("utf-8")


def build(input_dir, output_dir):
    output_dir.mkdir(parents=True, exist_ok=True)
    cat_ids, cat_names = load_categories(input_dir / "categories.tsv")

    files = sorted(p for p in input_dir.rglob("*")
                   if p.is_file() and p.suffix.lower() in (".md", ".html", ".htm", ".txt")
                   and "img" not in p.relative_to(input_dir).parts)

    # First pass: titles, so guide-to-guide links can be resolved.
    parsed = []
    link_map = {}
    for path in files:
        text = path.read_text(encoding="utf-8", errors="replace")
        meta, body = parse_front_matter(text) if path.suffix.lower() == ".md" else ({}, text)
        title = meta.get("title") or path.stem.replace("_", " ").replace("-", " ").title()
        parsed.append([path, meta, body, title])

    # Titles must be unique: add the category name to any that repeat.
    counts = {}
    for p in parsed:
        counts[p[3].lower()] = counts.get(p[3].lower(), 0) + 1
    for p in parsed:
        if counts[p[3].lower()] > 1:
            p[3] = f"{p[3]} ({cat_names.get(p[1].get('category', ''), 'other')})"
        link_map[p[0].stem] = p[3]

    articles = []
    seen = set()
    for path, meta, body, title in parsed:
        key = title.lower()
        if key in seen:
            print(f"! duplicate title skipped: {title}")
            continue
        seen.add(key)
        if path.suffix.lower() == ".md":
            html_body = render(body, link_map)
        elif path.suffix.lower() == ".txt":
            html_body = "<pre style='white-space:pre-wrap'>" + html.escape(body) + "</pre>"
        else:
            html_body = body
        category = meta.get("category", "")
        cat = cat_ids.index(category) if category in cat_ids else NO_CATEGORY
        if category and category not in cat_ids:
            print(f"! {path.name}: unknown category '{category}'")
        articles.append({
            "title": title,
            "title_bytes": clip_title(title),
            "html": html_body.encode("utf-8"),
            "cat": cat,
            "tags": meta.get("tags", ""),
            "summary": meta.get("summary", ""),
        })

    # Sorted by title: this is what makes the firmware's binary search work.
    # The firmware lowercases ASCII only and compares bytes, so sort the same way.
    def sort_key(a):
        return bytes(b + 32 if 65 <= b <= 90 else b for b in a["title_bytes"])
    articles.sort(key=sort_key)

    dat = bytearray()
    idx = bytearray(struct.pack("<IIII", MAGIC, len(articles), VERSION, 0))
    search_lines = []
    for a in articles:
        offset, length = len(dat), len(a["html"])
        dat += a["html"]
        idx += struct.pack("<56sIIB3x", a["title_bytes"], offset, length, a["cat"])
        search_lines.append("\t".join(
            s.replace("\t", " ").replace("\n", " ") for s in (a["title"], a["tags"], a["summary"])))

    (output_dir / "wiki.idx").write_bytes(idx)
    (output_dir / "wiki.dat").write_bytes(dat)
    (output_dir / "search.tsv").write_text("\n".join(search_lines) + "\n", encoding="utf-8", newline="\n")

    cat_src = input_dir / "categories.tsv"
    if cat_src.exists():
        shutil.copyfile(cat_src, output_dir / "categories.tsv")
    img_src = input_dir / "img"
    if img_src.is_dir():
        shutil.copytree(img_src, output_dir / "img", dirs_exist_ok=True)

    print(f"Indexed {len(articles)} articles")
    print(f"  wiki.dat   {len(dat) / 1024:.1f} KB")
    print(f"  wiki.idx   {len(idx) / 1024:.1f} KB")
    print(f"Copy the '{output_dir.name}' folder to the root of your FAT32/exFAT microSD card.")


def main():
    ap = argparse.ArgumentParser(description="Build the XiaoVault wiki files for the SD card")
    ap.add_argument("--input", default="guides", help="folder of guides (default: guides)")
    ap.add_argument("--output", default="sd_card/wiki", help="output folder (default: sd_card/wiki)")
    args = ap.parse_args()
    build(Path(args.input), Path(args.output))


if __name__ == "__main__":
    main()
