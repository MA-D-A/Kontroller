# Kontroller — ESP32 Robot Motor Controller

## Overview
Firmware arcade drive 3 motor untuk ESP32. Menerima input gamepad (stick LX/LY + bitmask tombol) dari laptop lewat USB serial, lalu menggerakkan dua motor differential + satu omni wheel.

Laptop menjalankan `laptop_bridge.py`, yang membaca gamepad lewat pygame dan mengirim baris CSV ke ESP32:

```
LX,LY,MASK\n
```

## Project Structure
```
Kontroller/
├── src/
│   └── Kontroller.ino    # Firmware (Arduino-compatible)
├── platformio.ini        # PlatformIO config (env: esp32dev)
├── laptop_bridge.py      # Gamepad → serial bridge (laptop side)
├── README.md
└── LICENSE
```

## Quick Start

### Option A: VS Code + PlatformIO (Recommended)
1. Open this folder in VS Code.
2. Install the **PlatformIO IDE** extension.
3. Build: `Ctrl+Alt+B` (or the ✓ icon in the status bar).
4. Upload: `Ctrl+Alt+U` (or the → icon).
5. Serial monitor: the 🔌 icon in the status bar (115200 baud).

> `F5` starts a *debug* session, which needs a JTAG probe. Standard ESP32 dev boards can't be debugged over USB alone, so use Upload instead.

### Option B: Arduino IDE
1. Install the ESP32 core: **File → Preferences → Additional Boards Manager URLs** → add
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`,
   then **Tools → Board → Boards Manager** → install **esp32 by Espressif Systems**.
2. Open `src/Kontroller.ino`.
   Arduino IDE requires a sketch to live in a folder with the same name. If it asks to move the file into a `Kontroller/` folder, do it in a **local copy only**; don't commit that move.
3. **Tools → Board → ESP32 Dev Module**
4. **Tools → Port** → select your ESP32 port.
5. Click **Upload** (→).

### Option C: CLI (PlatformIO Core)
```bash
pio run -e esp32dev              # Build
pio run -e esp32dev -t upload    # Upload
pio device monitor -b 115200     # Serial monitor
```

### Laptop Bridge (Gamepad → ESP32)
Requires Python 3.8+.

```bash
pip install pygame pyserial
python3 laptop_bridge.py [PORT]
```

- Without `PORT`, the bridge auto-detects `/dev/ttyUSB*` or `/dev/ttyACM*` (Linux).
- Pass the port explicitly on other OSes or when several devices are connected:
  - Linux: `python3 laptop_bridge.py /dev/ttyUSB0`
  - macOS: `python3 laptop_bridge.py /dev/cu.usbserial-XXXX`
  - Windows: `python laptop_bridge.py COM3`
- Baud rate: **115200**.
- Close any serial monitor first. Only one program can hold the port at a time.

## Hardware / Wiring

| Function     | ESP32 GPIO | Driver Pin |
|--------------|-----------:|------------|
| Motor A PWM  | 14 | ENA |
| Motor A Dir1 | 26 | IN1 |
| Motor A Dir2 | 27 | IN2 |
| Motor B PWM  | 25 | ENB |
| Motor B Dir1 | 32 | IN3 |
| Motor B Dir2 | 33 | IN4 |
| Motor C PWM  | 18 | ENC |
| Motor C Dir1 | 19 | IN5 |
| Motor C Dir2 | 21 | IN6 |

- Motors A & B: differential drive. Motor C: omni wheel.
- PWM: 1 kHz, 8-bit resolution (0–255).
- Max duty: **180 / 255**.
- Deadzone: **±5** on the -100..100 input scale. Values inside it are treated as 0.
- Power the motors from a separate supply, **not** from the ESP32 3V3/5V pin.
- The ESP32 GND and the motor driver GND **must be connected** (common ground).

## Communication Protocol

**Laptop → ESP32** (115200 baud, one line per update, terminated with `\n`):
```
LX,LY,MASK\n
```

| Field | Range | Meaning |
|-------|-------|---------|
| `LX`  | -100..100 | Turn. Positive = right |
| `LY`  | -100..100 | Throttle. Positive = forward |
| `MASK`| unsigned 32-bit | Button bitmask. Bit *n* = button *n* pressed |

Examples:
- `0,50,0` → forward at half speed, no turn, no buttons
- `-30,0,5` → turn left in place, buttons 0 and 2 pressed (`0b101`)

**ESP32 → Laptop** (debug echo after each parsed line):
```
LX=0 LY=50 BTN=0x0000 []
```

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `Permission denied: /dev/ttyUSB0` (Linux) | `sudo usermod -aG dialout $USER`, then log out and back in |
| Upload stuck at `Connecting....` | Hold the **BOOT** button on the board until upload starts |
| Port busy / bridge can't open port | Close the serial monitor or Arduino IDE Serial Monitor |
| No port appears | Install the USB driver for your board (CP210x or CH340) and use a data USB cable, not a charge-only one |
| `ledcSetup` / `ledcAttachPin` not declared | ESP32 Arduino core 3.x changed the LEDC API. Use core 2.x, or port the code to `ledcAttach()` / `ledcWrite(pin, duty)` |
| A motor spins the wrong way | Swap its two motor wires, or swap its `IN` pin definitions in code |

## Contributing — Step by Step

### 1. Fork & Clone
Fork the repo on GitHub, then:
```bash
git clone https://github.com/<YOUR-USERNAME>/Kontroller.git
cd Kontroller
git remote add upstream https://github.com/<ORIGINAL-OWNER>/Kontroller.git
```

### 2. Create a Branch
Always branch from an up-to-date `main`:
```bash
git checkout main
git pull upstream main
git checkout -b <type>/<short-description>
# Types: feat, fix, refactor, docs, test, chore
# Example: git checkout -b feat/add-battery-monitor
```

### 3. Make Changes
- Edit code in `src/`. Keep `Kontroller.ino` Arduino IDE-compatible.
- Update `platformio.ini` if you add libraries, and list them in this README for Arduino IDE users.
- If the protocol changes, update `laptop_bridge.py` **and** the Protocol section in this README in the same PR.

### 4. Test Locally
```bash
pio run -e esp32dev        # must build without errors
python3 laptop_bridge.py   # hardware test with a gamepad
```
Also confirm the sketch compiles in Arduino IDE (**Verify**).

### 5. Commit
Follow [Conventional Commits](https://www.conventionalcommits.org/):
```bash
git add <files>
git commit -m "<type>: <imperative summary>"
```
Examples:
```
feat: add battery voltage monitoring via ADC
fix: correct motor direction reversal on omni wheel
docs: update wiring table for v2 PCB
```
Add a body explaining *what* and *why* for non-trivial changes. Mark breaking changes with `BREAKING CHANGE:` in the body.

### 6. Push & Open a PR
```bash
git push -u origin <branch-name>
```
Open a pull request on GitHub against `main`, describe the change and how you tested it, then request review.

### 7. Review & Merge
- Address review comments by pushing new commits to the same branch.
- A maintainer merges (squash or rebase).
- Delete the branch after merge.

## Adding a New Feature
1. **Design**: describe any protocol or pin changes in the PR description.
2. **Implement**: modify `src/Kontroller.ino` (+ `platformio.ini` if needed).
3. **Update bridge**: modify `laptop_bridge.py` if the protocol changes.
4. **Test**: build in PlatformIO and Arduino IDE, then test on hardware.
5. **Document**: update the Wiring / Protocol sections of this README.

## Fixing a Bug
1. **Reproduce**: capture serial logs and bridge output.
2. **Isolate**: find a minimal way to trigger it.
3. **Fix**: edit the relevant file.
4. **Verify**: build in both IDEs and confirm there's no regression on hardware.
5. **Commit**: `fix: <description>`, and reference the issue (`Fixes #12`) if one exists.

## Updating Documentation
1. Edit `README.md`.
2. Commit with `docs: <description>`.
3. No hardware test needed.

## Code Style
- Arduino/PlatformIO defaults, 2-space indent.
- Pin definitions as `#define` at the top of `src/Kontroller.ino`.
- Functions: `lowerCamelCase`.
- Constants: `UPPER_SNAKE_CASE`.

## License
MIT — see [LICENSE](LICENSE).