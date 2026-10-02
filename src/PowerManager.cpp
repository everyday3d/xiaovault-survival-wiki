#include "PowerManager.h"
#include "WiFiApManager.h"

PowerManager& PowerManager::instance() {
    static PowerManager s_instance;
    return s_instance;
}

PowerManager::PowerManager() 
    : _lastActivityMs(0), _lastBatteryCheckMs(0), _cachedVoltage(0.0f) {
}

void PowerManager::begin() {
    pinMode(BATTERY_ADC_PIN, INPUT);
    pinMode(USER_BTN_PIN, INPUT_PULLUP);
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, HIGH); // LED ON

    _lastActivityMs = millis();
    getBatteryVoltage();
    Serial.println(F("[POWER] Power Manager initialized."));
}

float PowerManager::getBatteryVoltage() {
    if (millis() - _lastBatteryCheckMs < 2000 && _cachedVoltage > 0.0f) {
        return _cachedVoltage;
    }
    _lastBatteryCheckMs = millis();

    // Read ADC value (12-bit = 0 to 4095, ref ~3.3V)
    uint32_t raw = analogRead(BATTERY_ADC_PIN);
    float pinVoltage = (raw / 4095.0f) * 3.3f;

    // Voltage divider ratio: V_batt = pinVoltage * (R1 + R2) / R2
    float dividerRatio = (BATTERY_R1 + BATTERY_R2) / BATTERY_R2;
    float battVoltage = pinVoltage * dividerRatio;

    // Filter out floating pin noise if no battery is connected
    if (battVoltage < 2.0f) {
        _cachedVoltage = 0.0f; // USB Powered / No battery
    } else {
        _cachedVoltage = battVoltage;
    }

    return _cachedVoltage;
}

int PowerManager::getBatteryPercentage() {
    float v = getBatteryVoltage();
    if (v <= 0.0f) return 100; // USB powered
    if (v >= 4.20f) return 100;
    if (v <= 3.30f) return 0;

    // Approximate LiPo discharge curve
    int pct = (int)((v - 3.30f) / (4.20f - 3.30f) * 100.0f);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    return pct;
}

void PowerManager::notifyActivity() {
    _lastActivityMs = millis();
}

void PowerManager::loop() {
    if (AUTO_SLEEP_TIMEOUT_SEC <= 0) return;

    // Check if any clients are connected
    if (WiFiApManager::instance().getClientCount() > 0) {
        _lastActivityMs = millis();
        return;
    }

    // If no clients connected and timeout exceeded: enter deep sleep
    if (millis() - _lastActivityMs > ((unsigned long)AUTO_SLEEP_TIMEOUT_SEC * 1000UL)) {
        Serial.println(F("[POWER] 💤 Inactivity timeout reached. Entering Deep Sleep..."));
        enterDeepSleep();
    }
}

void PowerManager::enterDeepSleep() {
    digitalWrite(STATUS_LED_PIN, LOW); // LED OFF

    // Configure wakeup on button press
    esp_deep_sleep_enable_gpio_wakeup(1ULL << USER_BTN_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);

    Serial.println(F("[POWER] Press button to wake. Nighty night!"));
    Serial.flush();
    esp_deep_sleep_start();
}
