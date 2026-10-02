#include "WiFiApManager.h"

WiFiApManager& WiFiApManager::instance() {
    static WiFiApManager s_instance;
    return s_instance;
}

WiFiApManager::WiFiApManager() : _running(false) {
}

bool WiFiApManager::begin() {
    Serial.println(F("[WIFI] Setting up Offline Access Point..."));

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_GW, WIFI_AP_SUBNET);

    bool ok = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL, 0, WIFI_AP_MAX_CLIENTS);
    if (!ok) {
        Serial.println(F("[WIFI] ❌ Failed to start Access Point!"));
        _running = false;
        return false;
    }

    // Configure Captive Portal DNS Server
    // Redirect all DNS requests (*) to the ESP32 AP IP address (192.168.4.1)
    _dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    _dnsServer.start(DNS_PORT, "*", WIFI_AP_IP);

    _running = true;
    Serial.printf("[WIFI] ✅ Offline AP Live! SSID: \"%s\"\n", WIFI_AP_SSID);
    Serial.printf("[WIFI] Access Point IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.println(F("[WIFI] Captive Portal DNS active on port 53."));
    return true;
}

void WiFiApManager::loop() {
    if (_running) {
        _dnsServer.processNextRequest();
    }
}

uint8_t WiFiApManager::getClientCount() {
    return WiFi.softAPgetStationNum();
}

IPAddress WiFiApManager::getIpAddress() {
    return WiFi.softAPIP();
}
