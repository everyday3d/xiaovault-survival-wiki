# 🧊 XiaoVault C6 Tandem Edition — 3D Models & Slicer Guide

This directory contains the production-ready 3D CAD models, STEP files, and pre-oriented STLs for the **XiaoVault C6 Tandem Edition** snap-fit enclosure.

Designed specifically for the [XiaoVault Survival Wiki](https://github.com/everyday3d/xiaovault-survival-wiki), this case houses both the **Seeed Studio XIAO ESP32-C6** and a standard **MicroSD SPI breakout board** in an ultra-slim, pocket-friendly form factor.

---

## 📁 Included Files

| Filename | Format | Description |
| :--- | :--- | :--- |
| `1_XiaoVault_Tandem_Base_rev1.stl` | STL Mesh | Lower enclosure shell (pre-oriented floor-down for zero-support printing). |
| `2_XiaoVault_Tandem_Lid_rev1.stl` | STL Mesh | Upper snap-fit lid with GPIO & vent slots (pre-oriented roof-down for zero-support printing). |
| `1_XiaoVault_Tandem_Base_rev1.step` | STEP Solid | High-precision solid model of the base. |
| `2_XiaoVault_Tandem_Lid_rev1.step` | STEP Solid | High-precision solid model of the lid. |
| `XiaoVault_Tandem_Full_Assembly_rev1.step` | STEP Solid | Complete master assembly including Base, Lid, XIAO board, MicroSD board, and wire harness. |
| `XiaoVault_Tandem_Case.FCStd` | FreeCAD Project | Master parametric FreeCAD 1.1 source file. |

---

## 📐 Dimensions & Physical Specs

* **External Envelope:** $21.0\text{ mm}$ Wide $\times 44.5\text{ mm}$ Long $\times 9.4\text{ mm}$ High
* **Front Compartment (XIAO ESP32-C6):** $18.2\text{ mm} \times 21.5\text{ mm} \times 7.0\text{ mm}$ internal cavity
* **Rear Compartment (MicroSD Breakout):** $18.2\text{ mm} \times 19.0\text{ mm} \times 7.0\text{ mm}$ internal cavity
* **Internal Wire Channel:** $10.0\text{ mm}$ Wide $\times 2.4\text{ mm}$ High open U-channel through central divider
* **Rear MicroSD Slot:** $12.5\text{ mm}$ Wide $\times 2.4\text{ mm}$ High push-push card aperture
* **Front USB-C Port:** $9.8\text{ mm}$ split U-notch accommodating standard USB-C cable shrouds
* **Keyring / Paracord Loop:** $4.0\text{ mm}$ through-hole on rear-right corner (failsafe structural loop)
* **Wall Thickness:** $1.4\text{ mm}$ outer perimeter walls, $1.2\text{ mm}$ floor and roof

---

## 🖨️ Recommended Slicer Settings

This model is engineered for **100% Zero-Support FDM Printing**. Both parts are pre-oriented in their optimal print orientation.

* **Supports:** **DISABLED (None)** — Every ceiling, notch, and detent is self-supporting ($\le 26^\circ$ from vertical, or open U-channels).
* **Layer Height:** `0.16 mm` (Optimal) or `0.20 mm` (Standard)
* **Wall Loops:** `3` or `4` walls for maximum structural rigidity
* **Top/Bottom Shell Layers:** `4` top, `4` bottom
* **Infill:** `15% - 20%` (Gyroid, Grid, or Adaptive Cubic)
* **Material:** PLA, PLA+, PETG, or ABS/ASA
* **Print Bed Orientation:**
  * **Base:** Print floor-down flat on bed ($Z = 0$).
  * **Lid:** Print roof-down flat on bed ($Z = 0$).

---

## 🔧 Assembly Instructions

1. **Test Fit Boards:** Place the Seeed Studio XIAO ESP32-C6 into the front compartment and the MicroSD breakout board into the rear compartment to verify they rest flat on their respective $1.2\text{ mm}$ corner standoffs.
2. **Wire Interconnects:** Solder or crimp 6 flexible hookup wires or a flat ribbon between the XIAO and MicroSD breakout board:
   * `3V3` ➔ `VCC`
   * `GND` ➔ `GND`
   * `D8` ➔ `SCK` (Clock)
   * `D9` ➔ `MISO` (Master In, Slave Out)
   * `D10` ➔ `MOSI` (Master Out, Slave In)
   * `D3` ➔ `CS` (Chip Select)
3. **Route Harness:** Dress the 6 wires through the central divider channel ($10.0\text{ mm} \times 2.4\text{ mm}$).
4. **Close Lid:** Align the Lid with the Base (USB notch forward, SD slot rearward, lanyard tab matching). Press the lid firmly until all 8 detents click into place.
5. **Insert MicroSD:** Push your formatted MicroSD card into the rear ejection slot until it clicks into the push-push socket. To remove, simply push in slightly until it springs back out.
