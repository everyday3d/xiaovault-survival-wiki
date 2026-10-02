#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include "config.h"
#include "StorageEngine.h"
#include "WikiEngine.h"
#include "WiFiApManager.h"
#include "PowerManager.h"
#include "WebUI.h"
#include "EmergencyFallback.h"

WebServer server(80);

void handleRoot() {
    PowerManager::instance().notifyActivity();
    server.send_P(200, "text/html", INDEX_HTML);
}

void handleEmergencyApi() {
    PowerManager::instance().notifyActivity();
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (size_t i = 0; i < EMERGENCY_GUIDES_COUNT; i++) {
        JsonObject obj = arr.add<JsonObject>();
        obj["id"] = EMERGENCY_GUIDES[i].id;
        obj["title"] = EMERGENCY_GUIDES[i].title;
        obj["category"] = EMERGENCY_GUIDES[i].category;
        obj["icon"] = EMERGENCY_GUIDES[i].icon;
        obj["contentHtml"] = FPSTR(EMERGENCY_GUIDES[i].contentHtml);
    }

    String res;
    serializeJson(arr, res);
    server.send(200, "application/json", res);
}

void handleSearchApi() {
    PowerManager::instance().notifyActivity();
    String query = server.arg("q");
    String jsonRes = WikiEngine::instance().searchJson(query, 12);
    server.send(200, "application/json", jsonRes);
}

void handleWikiArticle() {
    PowerManager::instance().notifyActivity();
    String id = server.arg("id");
    if (id.length() == 0) {
        server.send(400, "text/plain", "Missing article id");
        return;
    }

    String html = WikiEngine::instance().getArticleHtml(id);
    server.send(200, "text/html", html);
}

void handleBrowse() {
    PowerManager::instance().notifyActivity();
    String path = server.arg("path");
    if (path.length() == 0) path = "/";

    String listHtml = StorageEngine::instance().listDirectoryHtml(path);
    
    String page = F("<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Browse SD</title><style>body{background:#0d1117;color:#c9d1d9;font-family:sans-serif;padding:20px;max-width:700px;margin:0 auto;}a{color:#58a6ff;}</style></head><body><p><a href='/'>← Back to XiaoVault Search</a></p>");
    page += listHtml;
    page += F("</body></html>");
    server.send(200, "text/html", page);
}

void handleViewFile() {
    PowerManager::instance().notifyActivity();
    String filePath = server.arg("file");
    if (filePath.length() == 0 || !StorageEngine::instance().fileExists(filePath)) {
        server.send(404, "text/plain", "File not found");
        return;
    }

    File f = StorageEngine::instance().openFile(filePath, "r");
    if (!f) {
        server.send(500, "text/plain", "Could not open file");
        return;
    }

    // Determine MIME type
    String contentType = "text/plain";
    if (filePath.endsWith(".html") || filePath.endsWith(".htm")) contentType = "text/html";
    else if (filePath.endsWith(".md")) contentType = "text/markdown";
    else if (filePath.endsWith(".jpg") || filePath.endsWith(".jpeg")) contentType = "image/jpeg";
    else if (filePath.endsWith(".png")) contentType = "image/png";
    else if (filePath.endsWith(".pdf")) contentType = "application/pdf";

    server.streamFile(f, contentType);
    f.close();
}

void handleStatusApi() {
    PowerManager::instance().notifyActivity();
    JsonDocument doc;

    StorageStatus sd = StorageEngine::instance().getStatus();
    doc["sd_mounted"] = sd.mounted;
    doc["sd_card_type"] = sd.cardTypeStr;
    doc["sd_total_mb"] = (uint32_t)(sd.totalBytes / (1024 * 1024));
    doc["sd_used_mb"] = (uint32_t)(sd.usedBytes / (1024 * 1024));
    doc["sd_free_mb"] = (uint32_t)((sd.totalBytes - sd.usedBytes) / (1024 * 1024));

    doc["battery_v"] = PowerManager::instance().getBatteryVoltage();
    doc["battery_pct"] = PowerManager::instance().getBatteryPercentage();
    doc["wifi_clients"] = WiFiApManager::instance().getClientCount();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["uptime_sec"] = millis() / 1000;
    doc["app_version"] = APP_VERSION;
    doc["articles_indexed"] = WikiEngine::instance().getArticleCount();

    String res;
    serializeJson(doc, res);
    server.send(200, "application/json", res);
}

// Captive Portal Handlers for iOS / Android / Windows
void handleCaptivePortal() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
}

void handleNotFound() {
    String host = server.hostHeader();
    // If client requested external domain (captive portal probe), redirect to 192.168.4.1
    if (host.indexOf("192.168.4.1") < 0 && host.indexOf("xiaovault.local") < 0) {
        handleCaptivePortal();
        return;
    }

    // Try serving from SD card
    String uri = server.uri();
    if (StorageEngine::instance().fileExists(uri)) {
        File f = StorageEngine::instance().openFile(uri, "r");
        server.streamFile(f, "text/plain");
        f.close();
        return;
    }

    server.send(404, "text/plain", "404 Not Found");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F("⚡ " APP_NAME " v" APP_VERSION));
    Serial.println(F("   " APP_TAGLINE));
    Serial.println(F("=================================================="));

    // 1. Initialize Power Manager (LED, Battery ADC, Sleep)
    PowerManager::instance().begin();

    // 2. Initialize MicroSD Storage Engine
    StorageEngine::instance().begin();

    // 3. Initialize Wiki Engine (Index reader & fallback)
    WikiEngine::instance().begin();

    // 4. Initialize WiFi AP & Captive Portal DNS
    WiFiApManager::instance().begin();

    // 5. Setup Web Server Endpoints
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/emergency", HTTP_GET, handleEmergencyApi);
    server.on("/api/search", HTTP_GET, handleSearchApi);
    server.on("/wiki", HTTP_GET, handleWikiArticle);
    server.on("/browse", HTTP_GET, handleBrowse);
    server.on("/view", HTTP_GET, handleViewFile);
    server.on("/api/status", HTTP_GET, handleStatusApi);

    // Captive portal probes
    server.on("/generate_204", HTTP_GET, handleCaptivePortal);       // Android
    server.on("/hotspot-detect.html", HTTP_GET, handleCaptivePortal); // iOS
    server.on("/ncsi.txt", HTTP_GET, handleCaptivePortal);           // Windows
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println(F("[HTTP] Web Server listening on port 80"));
    Serial.println(F("[SYSTEM] Ready! Connect to WiFi \"XiaoVault Survival Wiki\""));
}

void loop() {
    // Handle captive portal DNS requests
    WiFiApManager::instance().loop();

    // Handle HTTP client requests
    server.handleClient();

    // Power & sleep management
    PowerManager::instance().loop();
}
