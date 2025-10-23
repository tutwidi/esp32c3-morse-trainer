# ESP32-C3 Morse Trainer v7.0 – YD9CVS (Morse Tree Lite)

A lightweight visual Morse Code training tool built on **ESP32-C3**, featuring a **ST7735 TFT display** and **SI5351 frequency generator**.  
This version implements a *lite tree mode* visualization, auto-spacing logic, real-time running text, and tone/LED feedback.

---

## Features

- **Morse Tree Lite Display** — simplified visual tree (no connecting lines).  
- **Highlight Path** — active characters are highlighted as you key in.  
- **Auto-Spacing** — automatic detection of letter/word gaps.  
- **Running Text Display** — continuous decoded output at the screen bottom.  
- **Tone Bar Animation** — visual timing bar for DIT/DAH.  
- **LED + Buzzer Feedback** — synchronized tone and LED response.  
- **Configurable Parameters:**
  - `WPM` (Words per Minute)
  - `ToneHz` (CW tone frequency)

---

## Hardware Requirements

| Component | Description |
|------------|-------------|
| **MCU** | ESP32-C3 |
| **Display** | ST7735 1.8" TFT (SPI) |
| **Clock Generator** | SI5351 (I²C) |
| **Paddle Input** | Dual iambic keys (DIT/DAH) |
| **Audio Output** | Passive buzzer |
| **Visual Output** | LED indicator |

---

## Pinout

| Function | GPIO | Description |
|-----------|------|-------------|
| I²C SDA | 8 | SI5351 SDA |
| I²C SCL | 9 | SI5351 SCL |
| TFT CS | 7 | TFT Chip Select |
| TFT DC | 3 | TFT Data/Command |
| TFT RST | 5 | TFT Reset |
| TFT SCK | 10 | SPI Clock |
| TFT MOSI | 6 | SPI Data |
| Paddle DIT | 0 | Dot input |
| Paddle DAH | 1 | Dash input |
| Buzzer | 2 | CW tone output |
| LED | 4 | Status LED |

---

## 🧠 Software Overview

- **Platform:** Arduino IDE / PlatformIO  
- **Core Libraries:**
  - `Wire.h` — I²C communication  
  - `SPI.h` — SPI communication  
  - `Adafruit_GFX.h` & `Adafruit_ST7735.h` — TFT graphics rendering  
  - `si5351.h` — clock/tone generation  

### Key Functions

| Function | Purpose |
|-----------|----------|
| `drawHeader()` | Displays WPM, tone frequency, and decoded char |
| `drawTree()` | Renders Morse tree and highlights active path |
| `drawToneBar()` | Visualizes tone duration while keying |
| `drawRunningText()` | Shows decoded characters |
| `decodeMorse()` | Converts dot/dash strings to characters |
| `calcDit()` | Calculates unit timing from WPM |
| `beepStart()` / `beepStop()` | Audio and LED control |

---

## Display Layout

┌──────────────────────────────┐
│ WPM:15 Hz:750 A │ ← Header
├──────────────────────────────┤
│ Morse Tree │
│ Highlighted Path │
│ (A–Z, 0–9 Levels 1–5) │
├──────────────────────────────┤
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ← Tone bar
│ CQ TEST DE YD9CVS │ ← Running text
└──────────────────────────────┘

---

## Getting Started

1. **Connect** all components according to the pin table above.  
2. **Open** the `.ino` file in Arduino IDE.  
3. **Select Board:** `ESP32-C3 Dev Module`.  
4. **Install Libraries:**
   - Adafruit GFX Library  
   - Adafruit ST7735 and ST7789 Library  
   - si5351 Library  
5. **Upload** the sketch via USB.  
6. On boot, a startup screen appears:  

7. Begin keying Morse using the paddle — characters will be decoded in real time.

---

## Configuration

Edit parameters in the code as needed:
```cpp
int WPM = 15;     // Words per minute
int ToneHz = 750; // CW tone frequency

Timing auto-adjusts dynamically:

void calcDit() { ditTime = 1200 / WPM; }

License

MIT License
Copyright (c) 2025
Author: I Ketut Widiastawan (YD9CVS)

You are free to use, modify, and distribute this project for educational and amateur radio purposes.

Credits

Adafruit – display and graphics libraries
Si5351 Arduino Library – for CW tone generation
ARRL Morse Practice Methods – concept inspiration
Amateur radio community (ORARI, IARU, and CW operators)

🛰️ Version Info

Release: v7.0
Mode: Morse Tree Lite
Platform: ESP32-C3
Last Update: October 2025