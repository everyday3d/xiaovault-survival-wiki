#pragma once
#include <Arduino.h>
#include "config.h"

class PowerManager {
public:
    static PowerManager& instance();

    void begin();
    void loop();

    float getBatteryVoltage();
    int getBatteryPercentage();
    void notifyActivity();
    void enterDeepSleep();

private:
    PowerManager();
    unsigned long _lastActivityMs;
    unsigned long _lastBatteryCheckMs;
    float _cachedVoltage;
};
