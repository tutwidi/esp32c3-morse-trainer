# ESP32-C3 Morse Trainer v7.0 – Open Source Project for Radio Amateurs

## Re-Discover the Art of Morse Code – In a Modern Way

Morse code has never died — it has evolved.  
This project revives **the legacy of CW (Continuous Wave)** communication with a modern touch using **ESP32-C3**, **SI5351**, and **ST7735 TFT Display**.  
Designed and developed by **YD9CVS**, this open-source **Morse Trainer** blends *classic radio art* with *today’s embedded technology*.

---

## Why This Project Matters

For many amateur radio operators (HAMs), CW is more than just dots and dashes —  
it’s **a universal language**, **a mental exercise**, and **a discipline** that connects generations of operators.  

But learning CW has often been challenging:  
> Small keys, limited feedback, no visualization.

That’s where **ESP32-C3 Morse Trainer v7.0 (Morse Tree Lite)** steps in —  
bringing **visual learning, tone feedback, and decoding trees** into one compact educational device.

---

## What It Does

| Feature | Description |
|----------|-------------|
| **Real-Time Morse Visualization** | Shows a simplified *Morse tree* structure without lines, with highlighted paths while keying. |
| **Auto Spacing Logic** | Detects proper character and word spacing — perfect for CW rhythm training. |
| **Running Text Display** | Shows decoded characters in a scrolling text line. |
| **Tone + LED Feedback** | Plays tone and lights up LED simultaneously with key press. |
| **Compact Hardware** | Fits on one ESP32-C3 module + ST7735 TFT + SI5351 signal generator. |
| **Open Source** | 100% editable, forkable, and customizable. |

---

## Designed for Learning, Built for Experimenting

Whether you’re a:
- **Beginner** who just learned “dit” and “dah”  
- **Contester** polishing CW accuracy  
- **Homebrewer** integrating CW into SDR or APRS projects  

This trainer gives you instant, visual feedback for every key you send.

It even shows the path you’re taking across the Morse tree —  
helping you *see what you send*, and *feel the rhythm of CW*.

---

## Hardware Overview

| Module | Function |
|---------|-----------|
| **ESP32-C3** | Core microcontroller (Wi-Fi, BLE, RISC-V) |
| **ST7735 TFT (SPI)** | Display for Morse visualization |
| **SI5351 (I²C)** | Frequency/tone generator |
| **Dual Paddle Key** | Iambic input (DIT & DAH) |
| **Buzzer + LED** | Tone and visual indicators |

> *Note:* The SI5351 currently operates as a tone generator only —  
it’s **not yet capable of full RF transmission output**.  
Future revisions may include a low-power QRP driver stage.

---

## Example Display Layout
┌──────────────────────────────┐
│ WPM:15 Hz:750 E │
├──────────────────────────────┤
│ Morse Tree (A–Z, 0–9) │
│ Highlighted Path in Real-Time │
├──────────────────────────────┤
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │
│ CQ TEST DE YD9CVS │
└──────────────────────────────┘

---

## Contribute & Collaborate

We invite **all amateur radio operators, homebrewers, and embedded enthusiasts** to join in!

You can:
- Fork this repo and build your own version  
- Share improvements (timing, visualization, or hardware add-ons)  
- Add features such as:
  - CW sending trainer (TX mode)
  - Memory keyer
  - USB keyboard logging
  - QSO logger integration
  - Tiny QRP RF stage

> Each contribution — no matter how small — helps keep Morse alive in the digital era.

---

## The Vision Ahead

Imagine:
- Morse practice contests between local clubs  
- Integration with DMR/APRS hotspots  
- Logging and scoring directly via IPFS or blockchain-based QSO ledgers  
- Compact CW learning kits distributed to schools and ORARI members

That’s the long-term goal — turning this into a **community learning platform for modern HAMs**.

---

## How to Get Started

1. Clone or download this repository  
2. Wire up your hardware following the [Wiring Diagram](docs/wiring.png)  
3. Upload `morse_trainer_v7.ino` to your ESP32-C3  
4. Power up and start keying — see your CW come alive!

---

## Join the Project

**Repository:** [github.com/YD9CVS/esp32c3-morse-trainer](https://github.com/YD9CVS/esp32c3-morse-trainer)  
**Author:** *I Ketut Widiastawan (YD9CVS)*  
**Region:** ORARI Local Klungkung – Bali  
**Language:** English / Bahasa Indonesia  

> “The essence of amateur radio is experimentation, learning, and sharing.”  
> Let’s keep CW alive — one dot and dash at a time.

---

## License
Released under the **MIT License** – free for learning, teaching, and innovation.  
All we ask: credit the project and share your improvements!

---

