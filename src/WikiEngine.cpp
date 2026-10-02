#include "WikiEngine.h"
#include "StorageEngine.h"

WikiEngine& WikiEngine::instance() {
    static WikiEngine s_instance;
    return s_instance;
}

WikiEngine::WikiEngine() 
    : _hasIndex(false), _totalArticles(0) {
}

bool WikiEngine::begin() {
    StorageEngine& storage = StorageEngine::instance();
    if (!storage.isMounted()) {
        Serial.println(F("[WIKI] Storage not mounted. Using Flash Emergency Guides only."));
        _hasIndex = false;
        return false;
    }

    if (!storage.fileExists(WIKI_INDEX_FILE) || !storage.fileExists(WIKI_DATA_FILE)) {
        Serial.println(F("[WIKI] ⚠️ wiki.idx or wiki.dat not found in /wiki. Ready for raw guides."));
        _hasIndex = false;
        return false;
    }

    File idx = storage.openFile(WIKI_INDEX_FILE, "r");
    if (!idx) {
        _hasIndex = false;
        return false;
    }

    // Read header: [4 bytes MAGIC ("WIKI")][4 bytes total_articles]
    uint32_t magic = 0;
    idx.read((uint8_t*)&magic, sizeof(magic));
    if (magic != MWIKI_MAGIC) {
        Serial.printf("[WIKI] Invalid index magic: 0x%08X (expected 0x%08X)\n", magic, MWIKI_MAGIC);
        idx.close();
        _hasIndex = false;
        return false;
    }

    idx.read((uint8_t*)&_totalArticles, sizeof(_totalArticles));
    idx.close();

    _hasIndex = true;
    Serial.printf("[WIKI] ✅ Micro-Wiki Index Verified: %u articles indexed on SD card!\n", _totalArticles);
    return true;
}

uint32_t WikiEngine::fnv1a(const char* str) {
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (uint8_t)tolower(*str++);
        hash *= 16777619u;
    }
    return hash;
}

bool WikiEngine::binarySearch(const String& targetTitle, uint64_t& outOffset, uint32_t& outLength, String& outActualTitle) {
    if (!_hasIndex || _totalArticles == 0) return false;

    StorageEngine& storage = StorageEngine::instance();
    File idx = storage.openFile(WIKI_INDEX_FILE, "r");
    if (!idx) return false;

    String cleanTarget = targetTitle;
    cleanTarget.toLowerCase();
    cleanTarget.trim();

    int32_t low = 0;
    int32_t high = _totalArticles - 1;
    bool found = false;

    // Index header is 8 bytes: [magic: 4][count: 4]
    const size_t headerSize = 8;

    while (low <= high) {
        int32_t mid = low + (high - low) / 2;
        size_t entryPos = headerSize + ((size_t)mid * MWIKI_ENTRY_SIZE);

        if (!idx.seek(entryPos)) break;

        uint32_t hash = 0;
        char titleBuf[MWIKI_TITLE_MAX_LEN + 1] = {0};
        uint64_t offset = 0;
        uint32_t length = 0;

        idx.read((uint8_t*)&hash, 4);
        idx.read((uint8_t*)titleBuf, MWIKI_TITLE_MAX_LEN);
        titleBuf[MWIKI_TITLE_MAX_LEN] = '\0';
        idx.read((uint8_t*)&offset, 8);
        idx.read((uint8_t*)&length, 4);

        String midTitle = String(titleBuf);
        String midTitleLower = midTitle;
        midTitleLower.toLowerCase();
        midTitleLower.trim();

        int cmp = cleanTarget.compareTo(midTitleLower);
        if (cmp == 0 || midTitleLower.startsWith(cleanTarget)) {
            outOffset = offset;
            outLength = length;
            outActualTitle = midTitle;
            found = true;
            break;
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    idx.close();
    return found;
}

String WikiEngine::searchJson(const String& query, size_t maxResults) {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    String qLower = query;
    qLower.toLowerCase();
    qLower.trim();

    // 1. Search Flash Emergency Guides first (instant high-priority match)
    for (size_t i = 0; i < EMERGENCY_GUIDES_COUNT; i++) {
        String title = String(EMERGENCY_GUIDES[i].title);
        String titleLower = title;
        titleLower.toLowerCase();

        if (qLower.length() == 0 || titleLower.indexOf(qLower) >= 0) {
            JsonObject obj = arr.add<JsonObject>();
            obj["id"] = EMERGENCY_GUIDES[i].id;
            obj["title"] = EMERGENCY_GUIDES[i].title;
            obj["category"] = EMERGENCY_GUIDES[i].category;
            obj["icon"] = EMERGENCY_GUIDES[i].icon;
            if (arr.size() >= maxResults) break;
        }
    }

    // 2. Search SD Micro-Wiki index if mounted
    if (_hasIndex && arr.size() < maxResults) {
        uint64_t offset = 0;
        uint32_t length = 0;
        String matchTitle = "";

        if (binarySearch(query, offset, length, matchTitle)) {
            JsonObject obj = arr.add<JsonObject>();
            obj["id"] = "sd_" + matchTitle;
            obj["title"] = matchTitle;
            obj["category"] = "Wikipedia (SD)";
            obj["icon"] = "📚";
        }
    }

    String output;
    serializeJson(arr, output);
    return output;
}

String WikiEngine::getArticleHtml(const String& id) {
    // 1. Check if built-in flash emergency guide
    for (size_t i = 0; i < EMERGENCY_GUIDES_COUNT; i++) {
        if (id == EMERGENCY_GUIDES[i].id) {
            return String(FPSTR(EMERGENCY_GUIDES[i].contentHtml));
        }
    }

    // 2. Check if SD card article
    String title = id;
    if (title.startsWith("sd_")) {
        title = title.substring(3);
    }

    uint64_t offset = 0;
    uint32_t length = 0;
    String actualTitle = "";

    if (binarySearch(title, offset, length, actualTitle)) {
        StorageEngine& storage = StorageEngine::instance();
        File dataFile = storage.openFile(WIKI_DATA_FILE, "r");
        if (dataFile && dataFile.seek(offset)) {
            // Cap max buffer to 64KB for RAM safety
            size_t readLen = length > 65536 ? 65536 : length;
            std::unique_ptr<char[]> buf(new char[readLen + 1]);
            dataFile.read((uint8_t*)buf.get(), readLen);
            buf[readLen] = '\0';
            dataFile.close();

            String html = F("<h3>📚 ");
            html += actualTitle;
            html += F("</h3>");
            html += String(buf.get());
            return html;
        }
    }

    return F("<p style='color:#f85149'>Article not found on MicroSD card.</p>");
}
