# ⚡ XiaoVault Survival Wiki

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32--C6-orange.svg)](https://platformio.org/)
[![Hardware: Seeed Studio XIAO](https://img.shields.io/badge/Hardware-Seeed_XIAO_ESP32--C6-brightgreen.svg)](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html)
[![3D Case: 100% Zero Support](https://img.shields.io/badge/3D_Print-Zero_Supports-blue.svg)](models/)

> **A pocket-sized, self-hosted offline emergency knowledge base & Wikipedia micro-server running on the Seeed Studio XIAO ESP32-C6.**

When cellular towers, power grids, and internet access go down during emergencies, natural disasters, or remote off-grid expeditions, **XiaoVault Survival Wiki** provides instant, portable access to medical guides, survival manuals, and the entirety of human knowledge right on your smartphone or laptop.

---

## 📸 3D Printable Snap-Fit Case (XiaoVault C6)

This repository includes the complete 3D printable **XiaoVault C6** zero-support snap-fit enclosure files in [`models/`](models/):

![XiaoVault C6 Exploded Assembly](docs/xiao_case_exploded_view.png)

---

## 🌟 Key Features

* 📡 **Offline WiFi Hotspot & Instant Captive Portal**: The ESP32-C6 broadcasts its own standalone WiFi access point (`XiaoVault Survival Wiki`). When any phone connects, the browser automatically launches the search engine and reader. **Zero apps, zero cell towers, zero internet required.**
* ⚡ **High-Speed Micro-Wiki Binary Search**: Custom binary index algorithm searches through hundreds of thousands of articles on a MicroSD card in **less than 15 milliseconds** over SPI.
* 🚨 **Fail-Safe Flash Emergency Guides**: Even if your MicroSD card is lost, removed, or corrupted, life-saving emergency cheat sheets (Adult CPR, Tourniquet Application, Water Purification, Rule of Threes, and Morse Code) boot instantly directly from microcontroller flash memory (`PROGMEM`).
* 🔋 **Powered by Portable Phone Chargers (Zero Soldering!)**: Plug any standard USB-C power bank directly into the case. A compact 10,000mAh power bank provides **4+ days (90–100 hours)** of continuous, nonstop offline access. Includes smart keep-alive pulses to prevent power banks from auto-shutting off.
* 📱 **Ultra-Lightweight Dark-Mode Web App**: High-contrast, battery-saving dark interface (<15 KB) with instant search autocomplete, category filtering, and offline bookmarks saved to the phone's local storage.
* 🖨️ **100% Zero-Support Snap Enclosure**: Includes precision snap-fit CAD models with dual GPIO pin slots and an integrated 4.0 mm keyring fail-safe lock.

---

## 🏗️ System Architecture

```
                       +---------------------------------------+
                       |        Your Smartphone / Laptop       |
                       | (iOS, Android, Windows, Mac, Linux)   |
                       +---------------------------------------+
                                          ▲
                                          │ 📡 Local WiFi 6 (802.11 b/g/n/ax)
                                          │    SSID: "XiaoVault Survival Wiki"
                                          ▼
+---------------------------------------------------------------------------------+
|  Seeed Studio XIAO ESP32-C6 (160 MHz RISC-V)                                     |
|                                                                                 |
|  +--------------------+     +---------------------+     +--------------------+  |
|  | Captive Portal DNS |     | Lightweight Web Svr |     | Power Manager      |  |
|  | Port 53 (* -> AP)  |     | Port 80 (JSON & UI) |     | ADC Battery Sense  |  |
|  +--------------------+     +---------------------+     +--------------------+  |
|                                        │                                        |
|  +-------------------------------------+-------------------------------------+  |
|  | WikiEngine (Binary Search Parser & Flash Emergency Guide Fallback)        |  |
|  +-------------------------------------+-------------------------------------+  |
|                                        │ SPI Bus (25 MHz)                       |
+----------------------------------------+----------------------------------------+
                                         ▼
                 +-----------------------------------------------+
                 |  FAT32 / exFAT MicroSD Card (32GB – 128GB)    |
                 |  - /wiki/wiki.idx (High-speed binary index)   |
                 |  - /wiki/wiki.dat (Compressed article bodies) |
                 |  - /guides/       (Raw Markdown & HTML files) |
                 +-----------------------------------------------+
```

---

## 🚀 Quick Start Guide

### 1. Build and Flash the Firmware

This project uses [PlatformIO](https://platformio.org/). You can also use the Arduino IDE.

#### Option A: Using PlatformIO (Recommended)
```bash
# Clone the repository
git clone https://github.com/everyday3d/xiaovault-survival-wiki.git
cd xiaovault-survival-wiki

# Build and flash to Seeed Studio XIAO ESP32-C6
pio run -e seeed_xiao_esp32c6 -t upload

# Open serial monitor
pio device monitor -b 115200
```

#### Option B: Using Arduino IDE
1. Install the **ESP32** board package in the Arduino Boards Manager.
2. Select Board: **ESP32C6 Dev Module** (or Seeed Studio XIAO ESP32C6).
3. Install the **ArduinoJson** library (v7.x) via Library Manager.
4. Open [`src/main.cpp`](src/main.cpp) as a sketch and click **Upload**.

---

### 2. Prepare Your MicroSD Card

1. Format a MicroSD card (32GB or 64GB) as **FAT32** or **exFAT**.
2. Run the included Python tool to compile your survival guides or Wikipedia dumps into the high-speed index:
   ```bash
   cd tools
   python build_wiki_sd.py --create-samples
   ```
3. Copy the generated `wiki` folder directly onto the root of your MicroSD card:
   ```text
   MicroSD Card Root (D:\)
   └── wiki/
       ├── wiki.idx   (Binary Search Index)
       └── wiki.dat   (Article Content Data)
   ```
4. Insert the card into your MicroSD breakout board attached to the XIAO.

---

### 3. Connect and Read

1. Power on your XiaoVault device (via USB-C or attached LiPo battery).
2. On your phone or laptop, open your WiFi settings and connect to:
   * **Network Name (SSID)**: `XiaoVault Survival Wiki`
   * **Password**: *(None — Open network by default)*
3. A captive portal prompt will automatically appear! If it doesn't open automatically, open any browser and navigate to:
   ```text
   http://192.168.4.1/
   ```
4. Search, browse survival categories, and read offline guides!

---

## 📌 Wiring Diagram

Connect your MicroSD SPI card breakout to the Seeed Studio XIAO ESP32-C6 as follows:

| XIAO Pin Label | ESP32-C6 GPIO | MicroSD Module Pin | Description |
| :--- | :--- | :--- | :--- |
| **3V3** | 3.3V Power | **VCC** | 3.3V Power Supply |
| **GND** | Ground | **GND** | Ground |
| **D8** | `GPIO 19` | **SCK / CLK** | SPI Clock |
| **D9** | `GPIO 20` | **MISO / DO** | SPI Data Out |
| **D10** | `GPIO 21` | **MOSI / DI** | SPI Data In |
| **D7** | `GPIO 18` | **CS** | Chip Select |

For detailed battery and voltage divider wiring, see [`docs/WIRING.md`](docs/WIRING.md).

---

## 🧰 Hardware Bill of Materials (BOM)

* **Microcontroller**: [Seeed Studio XIAO ESP32-C6](https://www.amazon.com/dp/B0D2NKVB34) (~$5.20)
* **Storage**: 32GB or 64GB MicroSD Card (~$6.00)
* **SD Breakout**: 3.3V MicroSD Card SPI Module (~$1.50)
* **Power Source**: Any Standard **Portable Phone Charger / USB Power Bank** (5,000–10,000mAh) or solar charger
* **Case**: 3D Printed **XiaoVault C6** Snap Enclosure (Included in [`models/`](models/))

See [`docs/HARDWARE.md`](docs/HARDWARE.md) for full component links.

---

## 📜 License

This project is licensed under the **MIT License** — see the [`LICENSE`](LICENSE) file for details.

Developed with 🐾 for makers, hikers, and emergency preparedness by **everyday3d / BoxerDawg 3D**.
