#include "StorageEngine.h"

StorageEngine& StorageEngine::instance() {
    static StorageEngine s_instance;
    return s_instance;
}

StorageEngine::StorageEngine() 
    : _mounted(false), _spi(FSPI) {
}

bool StorageEngine::begin() {
    Serial.println(F("[SD] Initializing SPI bus for MicroSD..."));
    Serial.printf("[SD] Pins: SCK=%d, MISO=%d, MOSI=%d, CS=%d\n", 
                  SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

    // Initialize custom SPI pin mapping on ESP32
    _spi.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    // Mount SD card at 25MHz SPI speed
    if (!SD.begin(SD_CS_PIN, _spi, 25000000)) {
        Serial.println(F("[SD] ⚠️ SD Card Mount Failed. Running in Flash-Only Mode."));
        _mounted = false;
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println(F("[SD] ⚠️ No SD card detected in slot."));
        _mounted = false;
        return false;
    }

    _mounted = true;
    Serial.println(F("[SD] ✅ MicroSD Card successfully mounted!"));

    StorageStatus status = getStatus();
    Serial.printf("[SD] Card Type: %s | Total: %llu MB | Used: %llu MB\n", 
                  status.cardTypeStr.c_str(), 
                  status.totalBytes / (1024 * 1024), 
                  status.usedBytes / (1024 * 1024));

    return true;
}

StorageStatus StorageEngine::getStatus() {
    StorageStatus s;
    s.mounted = _mounted;
    s.totalBytes = 0;
    s.usedBytes = 0;
    s.cardTypeStr = "None";

    if (!_mounted) return s;

    s.cardType = SD.cardType();
    switch (s.cardType) {
        case CARD_MMC:  s.cardTypeStr = "MMC"; break;
        case CARD_SD:   s.cardTypeStr = "SDSC"; break;
        case CARD_SDHC: s.cardTypeStr = "SDHC/SDXC"; break;
        default:        s.cardTypeStr = "Unknown"; break;
    }

    s.totalBytes = SD.totalBytes();
    s.usedBytes = SD.usedBytes();
    return s;
}

File StorageEngine::openFile(const String& path, const char* mode) {
    if (!_mounted) return File();
    return SD.open(path, mode);
}

bool StorageEngine::fileExists(const String& path) {
    if (!_mounted) return false;
    return SD.exists(path);
}

String StorageEngine::listDirectoryHtml(const String& dirPath) {
    if (!_mounted) {
        return F("<p style='color:#f85149'>Error: MicroSD card is not mounted.</p>");
    }

    File root = SD.open(dirPath);
    if (!root || !root.isDirectory()) {
        return F("<p style='color:#f85149'>Directory not found.</p>");
    }

    String html = F("<div style='background:#161b22; border:1px solid #30363d; border-radius:8px; padding:16px;'>");
    html += F("<h3 style='color:#f0883e; margin-bottom:12px;'>📁 Directory: ");
    html += dirPath;
    html += F("</h3><ul style='list-style:none; padding:0;'>");

    if (dirPath != "/") {
        html += F("<li style='padding:6px 0; border-bottom:1px solid #21262d;'><a href='/browse?path=/'>📁 [Up to Root]</a></li>");
    }

    File file = root.openNextFile();
    int count = 0;
    while (file) {
        count++;
        html += F("<li style='padding:8px 0; border-bottom:1px solid #21262d; display:flex; justify-content:space-between;'>");
        if (file.isDirectory()) {
            html += F("<a href='/browse?path=");
            html += file.path();
            html += F("' style='color:#58a6ff; text-decoration:none;'>📁 ");
            html += file.name();
            html += F("</a><span style='color:#8b949e; font-size:0.8rem;'>DIR</span>");
        } else {
            html += F("<a href='/view?file=");
            html += file.path();
            html += F("' style='color:#c9d1d9; text-decoration:none;'>📄 ");
            html += file.name();
            html += F("</a><span style='color:#8b949e; font-size:0.8rem;'>");
            html += String(file.size() / 1024);
            html += F(" KB</span>");
        }
        html += F("</li>");
        file = root.openNextFile();
    }

    if (count == 0) {
        html += F("<li style='color:#8b949e; padding:10px 0;'>Empty directory.</li>");
    }

    html += F("</ul></div>");
    return html;
}
