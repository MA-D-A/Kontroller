# Kontroller — ESP32 Robot Motor Controller

## Overview
3-motor arcade drive firmware for ESP32. Receives gamepad input (LX/LY + button bitmask) from a laptop via USB serial and drives two differential motors + one omni wheel. Protocol: `LX,LY,MASK\n` (CSV, LX/LY = -100..100, MASK = button bitmask).

## Quick Start

### Option A: VS Code + PlatformIO (Recommended)
1. Open this folder in VS Code
2. Install **PlatformIO IDE** extension
3. Press `Ctrl+Alt+B` → Build
4. Press `F5` → Debug / Upload

### Option B: Arduino IDE
1. Open `src/Kontroller.ino` in Arduino IDE
2. Tools → Board → **ESP32 Dev Module**
3. Tools → Port → select your ESP32 port
4. Click **Upload** (→)

### Option C: CLI (PlatformIO Core)
```bash
pio run -e esp32dev              # Build
pio run -e esp32dev -t upload    # Upload
pio device monitor -b 115200     # Serial monitor
```

### Laptop Bridge (Gamepad → ESP32)
```bash
pip install pygame pyserial
python3 laptop_bridge.py [/dev/ttyUSB0]
```
- Auto-detects `/dev/ttyUSB*` or `/dev/ttyACM*`
- Pass port explicitly if multiple devices

## Hardware / Wiring
| Function | Pin | Label |
|----------|-----|-------|
| Motor A PWM | 14 | ENA |
| Motor A Dir1 | 26 | IN1 |
| Motor A Dir2 | 27 | IN2 |
| Motor B PWM | 25 | ENB |
| Motor B Dir1 | 32 | IN3 |
| Motor B Dir2 | 33 | IN4 |
| Motor C PWM | 18 | ENC |
| Motor C Dir1 | 19 | IN5 |
| Motor C Dir2 | 21 | IN6 |

- PWM: 1 kHz, 8-bit resolution
- Max speed: 180/255
- Deadzone: ±5

## Communication Protocol
```
LX,LY,MASK\n
```
- **LX**: -100..100 (left/right, positive = right)
- **LY**: -100..100 (forward/back, positive = forward)
- **MASK**: Unsigned long bitmask (bit 0 = button 0, bit 1 = button 1, …)
- **Example**: `0,50,0` → forward half speed, no turn, no buttons

ESP32 replies with debug line:
```
LX=0 LY=50 BTN=0x0000 []
```

## Contributing — Step by Step

### 1. Fork & Clone
```bash
git clone https://github.com/<YOUR-USERNAME>/Kontroller.git
cd Kontroller
```

### 2. Create Branch
```bash
git checkout -b <type>/<short-description>
# Types: feat, fix, refactor, docs, test, chore
# Example: git checkout -b feat/add-battery-monitor
```

### 3. Make Changes
- Edit code in `src/` (keep `.ino` Arduino-compatible)
- Update `platformio.ini` if adding libraries
- Update `laptop_bridge.py` if protocol changes
- Add tests if applicable

### 4. Test Locally
```bash
# PlatformIO
pio run -e esp32dev

# Arduino IDE: Open src/Kontroller.ino → Verify/Upload

# Hardware test: run laptop_bridge.py with gamepad
python3 laptop_bridge.py
```

### 5. Commit
```bash
git add .
git commit -m "<type>: <imperative summary>

<optional details: what changed, why, any breaking changes>"
# Examples:
# feat: add battery voltage monitoring via ADC
# fix: correct motor direction reversal on omni wheel
# docs: update wiring table for v2 PCB
```

### 6. Push & Open PR
```bash
git push -u origin <branch-name>
# Open PR on GitHub → fill template → request review
```

### 7. PR Review & Merge
- Address review comments (push new commits to same branch)
- Maintainer merges (squash or rebase)
- Delete branch after merge

## Adding a New Feature
1. **Design**: Document protocol/pin changes in PR description
2. **Implement**: Modify `src/Kontroller.ino` + `platformio.ini` if needed
3. **Update bridge**: Modify `laptop_bridge.py` if protocol changes
4. **Test**: Both IDEs + hardware
5. **Document**: Update README wiring/protocol sections

## Fixing a Bug
1. **Reproduce**: Capture serial logs / bridge output
2. **Isolate**: Minimal test case
3. **Fix**: Edit `src/Kontroller.ino`
4. **Verify**: Test both IDEs, confirm no regression
5. **Commit**: `fix: <description>` + link issue if exists

## Updating Documentation
1. Edit `README.md` directly
2. Commit: `docs: <description>`
3. No hardware test needed

## Code Style
- Arduino/PlatformIO defaults (2-space indent)
- Pin definitions as `#define` at top of `src/Kontroller.ino`
- Functions: `lowerCamelCase`
- Constants: `UPPER_SNAKE_CASE`

## License
MIT — see [LICENSE](LICENSE)