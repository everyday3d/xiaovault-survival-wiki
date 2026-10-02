#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include "config.h"

class WiFiApManager {
public:
    static WiFiApManager& instance();

    bool begin();
    void loop();
    uint8_t getClientCount();
    IPAddress getIpAddress();

private:
    WiFiApManager();
    DNSServer _dnsServer;
    bool _running;
};
