#pragma once
#include <Arduino.h>

// =============================================================================
// XIAOVAULT SURVIVAL WIKI — SYSTEM CONFIGURATION
// =============================================================================

#define APP_NAME            "XiaoVault Survival Wiki"
#define APP_VERSION         "1.0.0"
#define APP_TAGLINE         "Offline Emergency Knowledge Base & Wikipedia Micro-Server"

// -----------------------------------------------------------------------------
// 1. PIN DEFINITIONS (Optimized for Seeed Studio XIAO ESP32-C6)
// -----------------------------------------------------------------------------
#if defined(BOARD_XIAO_ESP32C6)
    // Seeed Studio XIAO ESP32-C6 Standard Pinout
    // SPI Bus for MicroSD Card Breakout
    #define SD_SCK_PIN      19  // D8 (SCK)
    #define SD_MISO_PIN     20  // D9 (MISO)
    #define SD_MOSI_PIN     18  // D10 (MOSI)
    #define SD_CS_PIN       2   // D2 (CS - Chip Select)

    // Optional Battery Voltage Divider (ADC)
    // Solder a 100k/100k voltage divider to monitor 3.7V LiPo battery
    #define BATTERY_ADC_PIN 0   // D0 / GPIO0
    #define BATTERY_R1      100000.0f
    #define BATTERY_R2      100000.0f

    // User / Wakeup Button (Active LOW)
    #define USER_BTN_PIN    9   // Onboard BOOT button / GPIO9

    // Status Indicator LED
    #define STATUS_LED_PIN  15  // D6 / GPIO15

#elif defined(BOARD_XIAO_ESP32S3)
    #define SD_SCK_PIN      7   // D8
    #define SD_MISO_PIN     8   // D9
    #define SD_MOSI_PIN     9   // D10
    #define SD_CS_PIN       4   // D7
    #define STATUS_LED_PIN  21
    #define BATTERY_ADC_PIN 1
    #define BATTERY_R1      100000.0f
    #define BATTERY_R2      100000.0f
    #define USER_BTN_PIN    0

#else
    #define SD_SCK_PIN      18
    #define SD_MISO_PIN     19
    #define SD_MOSI_PIN     23
    #define SD_CS_PIN       5
    #define STATUS_LED_PIN  2
    #define BATTERY_ADC_PIN 34
    #define BATTERY_R1      100000.0f
    #define BATTERY_R2      100000.0f
    #define USER_BTN_PIN    0
#endif

// -----------------------------------------------------------------------------
// 2. POWER BANK & USB POWER MANAGEMENT
// -----------------------------------------------------------------------------
// Many smart USB power banks auto-shutoff if a device draws under 50mA.
// Setting POWERBANK_KEEP_ALIVE to true emits periodic 200ms radio/LED pulses
// every 15 seconds to prevent smart power banks from going to sleep.
#define POWERBANK_KEEP_ALIVE        true
#define POWERBANK_PULSE_INTERVAL_MS 15000 // Every 15 seconds

// -----------------------------------------------------------------------------
// 2. OFFLINE WIFI ACCESS POINT SETTINGS
// -----------------------------------------------------------------------------
#define WIFI_AP_SSID        "XiaoVault Survival Wiki"
#define WIFI_AP_PASSWORD    ""                  // Empty string = Open WiFi network
#define WIFI_AP_CHANNEL     6                   // Standard 2.4 GHz channel
#define WIFI_AP_MAX_CLIENTS 8                   // Up to 8 simultaneous survival readers
#define WIFI_AP_IP          IPAddress(192, 168, 4, 1)
#define WIFI_AP_GW          IPAddress(192, 168, 4, 1)
#define WIFI_AP_SUBNET      IPAddress(255, 255, 255, 0)
#define DNS_PORT            53

// -----------------------------------------------------------------------------
// 3. POWER & TIMEOUT MANAGEMENT
// -----------------------------------------------------------------------------
// Automatically enter ultra-low power deep sleep if no WiFi clients are connected
// Set to 0 to disable auto-sleep (stay awake until manually powered down)
#define AUTO_SLEEP_TIMEOUT_SEC  900     // 15 minutes of idle time before deep sleep
#define BATTERY_CHECK_INTERVAL  10000   // Read battery voltage every 10 seconds

// -----------------------------------------------------------------------------
// 4. STORAGE & WIKI FILE LOCATIONS
// -----------------------------------------------------------------------------
#define WIKI_INDEX_FILE     "/wiki/wiki.idx"
#define WIKI_DATA_FILE      "/wiki/wiki.dat"
#define GUIDES_DIR          "/guides"
#define EMERGENCY_DIR       "/emergency"

// Micro-Wiki binary index entry size
// [4 bytes hash][28 bytes title (null-padded)][8 bytes offset][4 bytes length] = 44 bytes
#define MWIKI_MAGIC         0x57494B49 // "WIKI"
#define MWIKI_ENTRY_SIZE    44
#define MWIKI_TITLE_MAX_LEN 28
