# 🧰 Hardware Bill of Materials (BOM) — XiaoVault Survival Wiki

This project is designed to be affordable, modular, and easy to assemble using readily available maker hardware.

---

## 🛒 Core Components

| Component | Description | Approx. Price | Example Source |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | **Seeed Studio XIAO ESP32-C6** (WiFi 6, BLE 5, RISC-V) | ~$5.20 | [Amazon](https://www.amazon.com/dp/B0D2NKVB34) / [Seeed Studio](https://www.seeedstudio.com/Seeed-Studio-XIAO-ESP32C6-p-5884.html) |
| **MicroSD Module** | 3.3V MicroSD Card SPI Breakout Board or MicroSD Ribbon adapter | ~$1.50 | [Amazon](https://www.amazon.com/s?k=microsd+spi+breakout+3.3v) / AliExpress |
| **Storage Card** | 32GB or 64GB MicroSD Card (Class 10 / U1, formatted FAT32) | ~$5.00–$7.00 | SanDisk, Samsung, or Kingston |
| **Power Source** | **Any Standard Portable Phone Charger / USB Power Bank** (5,000–10,000mAh) | ~$10.00 (or one you already own) | Anker, INIU, Mi, or solar power banks |
| **Enclosure** | 3D Printed **XiaoVault C6** Snap-Fit Case (100% Zero-Support STL) | ~$0.30 in filament | [STL in repository](../models/) or MakerWorld |

**Total Estimated Hardware Cost**: **$12 – $18** *(Assuming you already own a phone charger!)*

---

## 💡 Alternative Hardware Options:
* **Seeed Studio Expansion Board for XIAO**:
  Seeed sells an official expansion board featuring a built-in MicroSD card slot, 0.96" OLED screen, buzzer, and RTC. This firmware runs directly on that setup with zero wiring!
* **Generic ESP32 Boards**:
  Any standard ESP32, ESP32-S3, or ESP32-C3 board with an SPI MicroSD card module will run this firmware out-of-the-box by selecting the target environment in `platformio.ini`.
