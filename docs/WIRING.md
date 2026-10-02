# ⚡ Wiring & Pinout Guide — XiaoVault Survival Wiki

This document details the exact pin connections between the **Seeed Studio XIAO ESP32-C6**, a standard **MicroSD Card SPI Module**, and an optional **3.7V Rechargeable LiPo Battery**.

---

## 📌 MicroSD Card Module to XIAO ESP32-C6 (SPI Bus)

A standard MicroSD card breakout board requires only 6 wires (Power, Ground, and 4 SPI lines):

```
+--------------------------+                  +------------------------+
| Seeed Studio XIAO        |                  | MicroSD Card SPI       |
| ESP32-C6                 |                  | Module                 |
|                          |                  |                        |
|                     3V3  |==================| VCC (3.3V)             |
|                     GND  |==================| GND                    |
|   D8  (GPIO 19)     SCK  |------------------| SCK / CLK              |
|   D9  (GPIO 20)     MISO |------------------| MISO / DO (Data Out)   |
|   D10 (GPIO 18)     MOSI |------------------| MOSI / DI (Data In)    |
|   D2  (GPIO 2)      CS   |------------------| CS (Chip Select)       |
+--------------------------+                  +------------------------+
```

### Pin Assignment Table:

| XIAO Pin Label | ESP32-C6 GPIO | MicroSD Pin | Function |
| :--- | :--- | :--- | :--- |
| **3V3** | 3.3V Power | **VCC** | Power Supply (Ensure module supports 3.3V) |
| **GND** | Ground | **GND** | Ground |
| **D8** | `GPIO 19` | **SCK / CLK** | SPI Serial Clock |
| **D9** | `GPIO 20` | **MISO / DO** | SPI Master In / Slave Out |
| **D10** | `GPIO 18` | **MOSI / DI** | SPI Master Out / Slave In |
| **D2** | `GPIO 2` | **CS** | Chip Select (Active LOW) |

> ⚠️ **Important Note on 5V vs 3.3V SD Modules**:
> Ensure your MicroSD card breakout does not have an aggressive 5V-to-3.3V regulator that causes brownouts on 3.3V input. Direct 3.3V MicroSD breakout adapters (or ribbon adapters) are strongly recommended.

---

## 🔋 Power Method 1: Portable Phone Charger / USB Power Bank (Recommended — Zero Soldering!)

The easiest, safest, and most practical way to power the XiaoVault Survival Wiki is with any **standard portable USB phone charger (power bank)**:

```
+---------------------------+       USB-C Cable       +----------------------------+
| Portable USB Power Bank   |========================>| XiaoVault C6 Case          |
| (5,000mAh – 20,000mAh)    |                         | (Seeed XIAO ESP32-C6)      |
+---------------------------+                         +----------------------------+
```

### Why a USB Phone Charger is Ideal:
* **Zero Soldering Required**: Just plug a standard USB-C cable directly into the USB-C cutout on the 3D-printed XiaoVault case.
* **Massive Multi-Day Runtime**:
  * A compact **5,000 mAh** bank powers the micro-server for **$\approx 45–50\text{ hours}$** of continuous reading.
  * A standard **10,000 mAh** bank runs it for **$\approx 90–100\text{ hours}$** (over 4 full days nonstop!).
  * If left idle with auto-sleep enabled, it can remain on standby for **weeks**.
* **Smart Power Bank Keep-Alive**: The firmware includes an active keep-alive pulse (`POWERBANK_KEEP_ALIVE`) that prevents smart power banks from turning off due to low current draw.
* **Universal Charging**: Recharge your power bank from a car adapter, solar panel, generator, or hand-crank charger in an emergency.

---

## 🔋 Power Method 2: Optional Soldered 3.7V LiPo Battery

If you want an ultra-slim, self-contained keychain unit without an external cable, you can solder a small 3.7V LiPo battery directly to the bottom pads of the XIAO ESP32-C6:

```
[ BOTTOM SIDE OF XIAO ESP32-C6 ]
  +-----------------------+
  |  [ + ]        [ - ]   |  <--- Dedicated Battery Solder Pads
  |  BAT+         BAT-    |
  +-----------------------+
       |            |
     ( + )        ( - )
  [ RED Wire ]  [ BLACK Wire ]
  +-------------------------------+
  |  3.7V LiPo Battery (e.g. 500mAh)
  +-------------------------------+
```

* When plugged into USB-C, the board automatically charges the attached LiPo battery.
* When disconnected from USB-C, the board automatically and seamlessly switches to battery power.

---

## 🎛️ Optional Battery Voltage Divider (ADC Sensing)

To monitor battery percentage on the web dashboard, connect a simple $100\text{k}\Omega / 100\text{k}\Omega$ resistor divider:

```
BAT+ (3.7V - 4.2V)
  │
 [100kΩ Resistor (R1)]
  │
  ├───────> Connect to D0 (GPIO 0 / ADC)
  │
 [100kΩ Resistor (R2)]
  │
 GND
```
* With this divider, $4.2\text{V}$ battery voltage is divided in half to $2.1\text{V}$, safe for the ESP32-C6 ADC.
* Configure `BATTERY_ADC_PIN 0` in [`src/config.h`](../src/config.h).
