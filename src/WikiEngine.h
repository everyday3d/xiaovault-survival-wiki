#pragma once
#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "EmergencyFallback.h"

struct WikiSearchResult {
    String id;
    String title;
    String category;
    String icon;
};

class WikiEngine {
public:
    static WikiEngine& instance();

    bool begin();
    bool isIndexLoaded() const { return _hasIndex; }
    uint32_t getArticleCount() const { return _totalArticles; }

    String searchJson(const String& query, size_t maxResults = 10);
    String getArticleHtml(const String& id);

private:
    WikiEngine();
    bool _hasIndex;
    uint32_t _totalArticles;

    // Helper binary search over index file
    bool binarySearch(const String& targetTitle, uint64_t& outOffset, uint32_t& outLength, String& outActualTitle);
    uint32_t fnv1a(const char* str);
};
