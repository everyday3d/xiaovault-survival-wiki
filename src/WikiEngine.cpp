#include "WikiEngine.h"
#include "StorageEngine.h"

WikiEngine& WikiEngine::instance() {
    static WikiEngine s_instance;
    return s_instance;
}

WikiEngine::WikiEngine()
    : _hasIndex(false), _hasSearchFile(false), _totalArticles(0) {
    memset(_catCounts, 0, sizeof(_catCounts));
}

bool WikiEngine::readEntry(File& idx, uint32_t n, WikiEntry& out) {
    uint8_t raw[MWIKI_ENTRY_SIZE];
    if (!idx.seek(MWIKI_HEADER_SIZE + (size_t)n * MWIKI_ENTRY_SIZE)) return false;
    if (idx.read(raw, MWIKI_ENTRY_SIZE) != MWIKI_ENTRY_SIZE) return false;
    memcpy(out.title, raw, MWIKI_TITLE_MAX_LEN);
    out.title[MWIKI_TITLE_MAX_LEN] = '\0';
    memcpy(&out.offset, raw + MWIKI_TITLE_MAX_LEN, 4);
    memcpy(&out.length, raw + MWIKI_TITLE_MAX_LEN + 4, 4);
    out.category = raw[MWIKI_TITLE_MAX_LEN + 8];
    return true;
}

bool WikiEngine::begin() {
    StorageEngine& storage = StorageEngine::instance();
    _hasIndex = false;
    _hasSearchFile = false;
    _totalArticles = 0;
    memset(_catCounts, 0, sizeof(_catCounts));

    if (!storage.isMounted()) {
        Serial.println(F("[WIKI] Storage not mounted. Using flash emergency guides only."));
        return false;
    }
    if (!storage.fileExists(WIKI_INDEX_FILE) || !storage.fileExists(WIKI_DATA_FILE)) {
        Serial.println(F("[WIKI] wiki.idx or wiki.dat not found in /wiki. Flash guides only."));
        return false;
    }

    File idx = storage.openFile(WIKI_INDEX_FILE, "r");
    if (!idx) return false;

    uint32_t head[4] = {0, 0, 0, 0};  // magic, count, version, reserved
    if (idx.read((uint8_t*)head, sizeof(head)) != sizeof(head) || head[0] != MWIKI_MAGIC) {
        Serial.printf("[WIKI] Invalid index magic: 0x%08X\n", (unsigned)head[0]);
        idx.close();
        return false;
    }
    if (head[2] != MWIKI_VERSION) {
        Serial.printf("[WIKI] Index version %u is not supported (need %u). Rebuild the card with tools/build_wiki_sd.py\n",
                      (unsigned)head[2], (unsigned)MWIKI_VERSION);
        idx.close();
        return false;
    }
    _totalArticles = head[1];

    // Count articles per category once, for the home screen tiles.
    if (_totalArticles <= MWIKI_SMALL_WIKI_LIMIT) {
        WikiEntry e;
        for (uint32_t i = 0; i < _totalArticles; i++) {
            if (readEntry(idx, i, e) && _catCounts[e.category] < 65535) _catCounts[e.category]++;
        }
        _hasSearchFile = storage.fileExists(WIKI_SEARCH_FILE);
    }
    idx.close();

    _hasIndex = true;
    Serial.printf("[WIKI] Index verified: %u articles on the SD card\n", (unsigned)_totalArticles);
    return true;
}

// Exact (case-insensitive) title lookup by binary search. Titles are stored sorted
// by ASCII-lowercase byte order, which is what compareTo() does on the lowered text.
bool WikiEngine::findExact(const String& title, WikiEntry& out) {
    if (!_hasIndex || _totalArticles == 0) return false;
    File idx = StorageEngine::instance().openFile(WIKI_INDEX_FILE, "r");
    if (!idx) return false;

    String target = title;
    target.toLowerCase();
    target.trim();

    int32_t low = 0, high = (int32_t)_totalArticles - 1;
    bool found = false;
    while (low <= high) {
        int32_t mid = low + (high - low) / 2;
        WikiEntry e;
        if (!readEntry(idx, mid, e)) break;
        String t = String(e.title);
        t.toLowerCase();
        int cmp = target.compareTo(t);
        if (cmp == 0) { out = e; found = true; break; }
        if (cmp < 0) high = mid - 1; else low = mid + 1;
    }
    idx.close();
    return found;
}

void WikiEngine::addSdResult(JsonArray& arr, const WikiEntry& e) {
    JsonObject obj = arr.add<JsonObject>();
    obj["id"] = String("sd_") + e.title;
    obj["title"] = e.title;
    obj["category"] = categoryName(e.category);
    obj["icon"] = "\xF0\x9F\x93\x96";  // book
}

String WikiEngine::categoryName(uint8_t category) {
    if (category == 255) return "Library";
    File f = StorageEngine::instance().openFile(WIKI_CATEGORIES_FILE, "r");
    if (!f) return "Library";
    String name = "Library";
    uint8_t row = 0;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.length() == 0 || line[0] == '#') continue;
        if (row == category) {
            int a = line.indexOf('\t');
            int b = line.indexOf('\t', a + 1);
            if (a > 0 && b > a) name = line.substring(a + 1, b);
            break;
        }
        row++;
    }
    f.close();
    return name;
}

String WikiEngine::searchJson(const String& query, size_t maxResults) {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    String q = query;
    q.toLowerCase();
    q.trim();

    // 1. Flash emergency guides first: instant, and they work with no SD card.
    for (size_t i = 0; i < EMERGENCY_GUIDES_COUNT; i++) {
        String titleLower = String(EMERGENCY_GUIDES[i].title);
        titleLower.toLowerCase();
        if (q.length() == 0 || titleLower.indexOf(q) >= 0) {
            JsonObject obj = arr.add<JsonObject>();
            obj["id"] = EMERGENCY_GUIDES[i].id;
            obj["title"] = EMERGENCY_GUIDES[i].title;
            obj["category"] = EMERGENCY_GUIDES[i].category;
            obj["icon"] = EMERGENCY_GUIDES[i].icon;
            if (arr.size() >= maxResults) break;
        }
    }

    if (!_hasIndex || q.length() == 0 || arr.size() >= maxResults) {
        String out;
        serializeJson(arr, out);
        return out;
    }

    StorageEngine& storage = StorageEngine::instance();
    File idx = storage.openFile(WIKI_INDEX_FILE, "r");

    if (idx && _hasSearchFile) {
        // Small wiki: every word must appear in the title or the keywords.
        // Title matches are listed before keyword-only matches.
        File sf = storage.openFile(WIKI_SEARCH_FILE, "r");
        if (sf) {
            const size_t room = maxResults - arr.size();
            std::vector<uint32_t> titleHits, tagHits;
            uint32_t n = 0;
            while (sf.available() && n < _totalArticles && titleHits.size() < room) {
                String line = sf.readStringUntil('\n');
                line.toLowerCase();
                int t1 = line.indexOf('\t');
                String title = t1 < 0 ? line : line.substring(0, t1);
                int t2 = t1 < 0 ? -1 : line.indexOf('\t', t1 + 1);
                String keys = t1 < 0 ? "" : (t2 < 0 ? line.substring(t1 + 1) : line.substring(t1 + 1, t2));

                bool allInTitle = true, allAnywhere = true;
                int pos = 0;
                while (pos < (int)q.length()) {
                    int sp = q.indexOf(' ', pos);
                    if (sp < 0) sp = q.length();
                    String w = q.substring(pos, sp);
                    pos = sp + 1;
                    if (w.length() == 0) continue;
                    bool inTitle = title.indexOf(w) >= 0;
                    if (!inTitle) allInTitle = false;
                    if (!inTitle && keys.indexOf(w) < 0) { allAnywhere = false; break; }
                }
                if (allAnywhere) {
                    if (allInTitle) titleHits.push_back(n);
                    else if (tagHits.size() < room) tagHits.push_back(n);
                }
                n++;
            }
            sf.close();
            WikiEntry e;
            for (uint32_t i : titleHits) {
                if (arr.size() >= maxResults) break;
                if (readEntry(idx, i, e)) addSdResult(arr, e);
            }
            for (uint32_t i : tagHits) {
                if (arr.size() >= maxResults) break;
                if (readEntry(idx, i, e)) addSdResult(arr, e);
            }
        }
    } else if (idx) {
        // Large wiki: binary search to the first title that starts with the query,
        // then list the run of titles that share the prefix.
        int32_t low = 0, high = (int32_t)_totalArticles;
        while (low < high) {
            int32_t mid = low + (high - low) / 2;
            WikiEntry e;
            if (!readEntry(idx, mid, e)) break;
            String t = String(e.title);
            t.toLowerCase();
            if (t.compareTo(q) < 0) low = mid + 1; else high = mid;
        }
        WikiEntry e;
        for (uint32_t i = low; i < _totalArticles && arr.size() < maxResults; i++) {
            if (!readEntry(idx, i, e)) break;
            String t = String(e.title);
            t.toLowerCase();
            if (!t.startsWith(q)) break;
            addSdResult(arr, e);
        }
    }
    if (idx) idx.close();

    String out;
    serializeJson(arr, out);
    return out;
}

String WikiEngine::categoriesJson() {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    if (_hasIndex) {
        File f = StorageEngine::instance().openFile(WIKI_CATEGORIES_FILE, "r");
        if (f) {
            uint8_t row = 0;
            while (f.available()) {
                String line = f.readStringUntil('\n');
                line.trim();
                if (line.length() == 0 || line[0] == '#') continue;
                int a = line.indexOf('\t');
                int b = line.indexOf('\t', a + 1);
                int c = line.indexOf('\t', b + 1);
                if (a > 0 && b > a) {
                    JsonObject o = arr.add<JsonObject>();
                    o["n"] = row;
                    o["id"] = line.substring(0, a);
                    o["name"] = line.substring(a + 1, b);
                    o["color"] = c > b ? line.substring(b + 1, c) : line.substring(b + 1);
                    if (c > b) o["desc"] = line.substring(c + 1);
                    o["count"] = _catCounts[row];
                }
                row++;
            }
            f.close();
        }
    }
    String out;
    serializeJson(arr, out);
    return out;
}

String WikiEngine::categoryJson(uint8_t category, size_t maxResults) {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    if (_hasIndex) {
        File idx = StorageEngine::instance().openFile(WIKI_INDEX_FILE, "r");
        if (idx) {
            String cname = categoryName(category);
            WikiEntry e;
            for (uint32_t i = 0; i < _totalArticles && arr.size() < maxResults; i++) {
                if (!readEntry(idx, i, e) || e.category != category) continue;
                JsonObject obj = arr.add<JsonObject>();
                obj["id"] = String("sd_") + e.title;
                obj["title"] = e.title;
                obj["category"] = cname;
                obj["icon"] = "\xF0\x9F\x93\x96";
            }
            idx.close();
        }
    }
    String out;
    serializeJson(arr, out);
    return out;
}

String WikiEngine::getArticleHtml(const String& id) {
    for (size_t i = 0; i < EMERGENCY_GUIDES_COUNT; i++) {
        if (id == EMERGENCY_GUIDES[i].id) {
            return String(FPSTR(EMERGENCY_GUIDES[i].contentHtml));
        }
    }

    String title = id;
    if (title.startsWith("sd_")) title = title.substring(3);

    WikiEntry e;
    if (findExact(title, e)) {
        File data = StorageEngine::instance().openFile(WIKI_DATA_FILE, "r");
        if (data && data.seek(e.offset)) {
            size_t readLen = e.length > 65536 ? 65536 : e.length;  // RAM safety cap
            std::unique_ptr<char[]> buf(new char[readLen + 1]);
            size_t got = data.read((uint8_t*)buf.get(), readLen);
            buf[got] = '\0';
            data.close();

            String html = F("<h3>");
            html += e.title;
            html += F("</h3>");
            html += String(buf.get());
            return html;
        }
    }
    return F("<p style='color:#f85149'>Article not found on the MicroSD card.</p>");
}
