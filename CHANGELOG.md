# Changelog

All notable changes to the **XiaoVault Survival Wiki** project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.1.0] - 2026-11-10

The guide library from the [ESP32 CYD Survival Wiki](https://github.com/everyday3d/esp32-cyd-survival-wiki)
comes to the XIAO ESP32-C6. Tested on a real board.

### Added
- **118 field guides on the SD card**, in 15 categories: First Steps, Water, Fire, Shelter, First Aid,
  Food, Navigation, Signals & Rescue, Weather & Storms, Vehicles & Roads, Home & Utilities,
  Radio & Comms, Kids & Family, Hygiene & Sanitation and Good Boy (dog emergencies). The Markdown
  sources are in [`guides/`](guides/); a ready-made card image is in [`sd_card/wiki/`](sd_card/wiki/).
- **Category tiles on the home screen**, read from `categories.tsv` on the card, each showing how many
  guides it holds. Tap one for its guide list. Without a card the built-in flash tiles stay.
- **Keyword search**: search now matches a guide's keywords as well as its title, and every word has to
  match. "cpr" finds the CPR guide, "tick" finds the bites guide. Title matches are listed first.
- **Links between guides** work inside the reader, and **pictures** (22 line drawings) show up in the guides.
- **Keep-the-title-short rule removed**: titles can now be up to 56 bytes (was 28).
- `tools/build_wiki_sd.py` reads the same front matter as the CYD project (`title`, `category`, `tags`,
  `summary`), turns Markdown into HTML, and writes the index, the data file, a keyword file, the category
  list and the pictures in one go. Run it with no arguments to rebuild `sd_card/wiki/` from `guides/`.

### Changed
- **Index format v2** (`wiki.idx`): 16-byte header with a version number, 68-byte entries that carry a
  category. **Rebuild your card** with `tools/build_wiki_sd.py`. Old v1 cards are ignored (the flash
  guides still work) and the serial log says why.
- Search returns several matches instead of one. The old prefix search could miss a guide.
- Files from the SD card are served with the right content type (pictures were sent as plain text).
- Version 1.1.0.

### Removed
- `sample_articles/` and `--create-samples`. The three sample articles are replaced by the full library.

---

## [1.0.0]

- First release: XIAO ESP32-C6 offline hotspot with a captive portal, a dark-mode web app, binary-indexed
  search over a microSD card, flash-resident emergency guides, power-bank keep-alive, a browser web
  flasher and the zero-support Tandem snap-fit case.
