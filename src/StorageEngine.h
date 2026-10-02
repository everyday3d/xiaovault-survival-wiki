#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include "config.h"

struct StorageStatus {
    bool mounted;
    uint8_t cardType;
    uint64_t totalBytes;
    uint64_t usedBytes;
    String cardTypeStr;
};

class StorageEngine {
public:
    static StorageEngine& instance();

    bool begin();
    bool isMounted() const { return _mounted; }
    StorageStatus getStatus();
    
    File openFile(const String& path, const char* mode = "r");
    bool fileExists(const String& path);
    String listDirectoryHtml(const String& dirPath);

private:
    StorageEngine();
    bool _mounted;
    SPIClass _spi;
};
