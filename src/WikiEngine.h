#pragma once
#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "EmergencyFallback.h"

// One row of wiki.idx (index format v2, 68 bytes on the card).
struct WikiEntry {
    char     title[MWIKI_TITLE_MAX_LEN + 1];
    uint32_t offset;
    uint32_t length;
    uint8_t  category;   // row number in categories.tsv, 255 = none
};

class WikiEngine {
public:
    static WikiEngine& instance();

    bool begin();
    bool isIndexLoaded() const { return _hasIndex; }
    uint32_t getArticleCount() const { return _totalArticles; }

    // Search flash guides plus the SD index. Matches titles and keywords.
    String searchJson(const String& query, size_t maxResults = 12);
    // Category tiles for the home screen (empty array if the card has none).
    String categoriesJson();
    // Articles in one category, in title order.
    String categoryJson(uint8_t category, size_t maxResults = 150);
    // HTML for a flash guide id or an SD article ("sd_<title>" or just the title).
    String getArticleHtml(const String& id);

private:
    WikiEngine();
    bool _hasIndex;
    bool _hasSearchFile;
    uint32_t _totalArticles;
    uint16_t _catCounts[256];

    bool readEntry(File& idx, uint32_t n, WikiEntry& out);
    bool findExact(const String& title, WikiEntry& out);
    void addSdResult(JsonArray& arr, const WikiEntry& e);
    String categoryName(uint8_t category);
};
