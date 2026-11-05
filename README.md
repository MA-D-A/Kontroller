# Kontroller — Kontroler Motor Robot ESP32

## Gambaran Umum
Firmware arcade drive 3 motor untuk ESP32. Menerima input gamepad (stick LX/LY + bitmask tombol) dari laptop lewat USB serial, lalu menggerakkan dua motor differential + satu omni wheel.

Laptop menjalankan `laptop_bridge.py`, yang membaca gamepad lewat pygame dan mengirim baris CSV ke ESP32:

```
LX,LY,MASK\n
```

## Struktur Proyek
```
Kontroller/
├── src/
│   └── Kontroller.ino    # Firmware (kompatibel Arduino)
├── platformio.ini        # Konfigurasi PlatformIO (env: esp32dev)
├── laptop_bridge.py      # Bridge gamepad → serial (sisi laptop)
├── README.md
└── LICENSE
```

## Mulai Cepat

### Opsi A: VS Code + PlatformIO (Disarankan)
1. Buka folder ini di VS Code.
2. Install ekstensi **PlatformIO IDE**.
3. Build: `Ctrl+Alt+B` (atau ikon ✓ di status bar).
4. Upload: `Ctrl+Alt+U` (atau ikon →).
5. Serial monitor: ikon 🔌 di status bar (115200 baud).

> `F5` menjalankan sesi *debug* yang butuh probe JTAG. Board dev ESP32 standar tidak bisa di-debug hanya lewat USB, jadi gunakan Upload.

### Opsi B: Arduino IDE
1. Install core ESP32: **File → Preferences → Additional Boards Manager URLs** → tambahkan
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`,
   lalu **Tools → Board → Boards Manager** → install **esp32 by Espressif Systems**.
2. Buka `src/Kontroller.ino`.
   Arduino IDE mengharuskan sketch berada di folder dengan nama yang sama. Kalau diminta memindahkan file ke folder `Kontroller/`, lakukan **hanya di salinan lokal**. Jangan commit perpindahan itu.
3. **Tools → Board → ESP32 Dev Module**
4. **Tools → Port** → pilih port ESP32 kamu.
5. Klik **Upload** (→).

### Opsi C: CLI (PlatformIO Core)
```bash
pio run -e esp32dev              # Build
pio run -e esp32dev -t upload    # Upload
pio device monitor -b 115200     # Serial monitor
```

### Laptop Bridge (Gamepad → ESP32)
Butuh Python 3.8+.

```bash
pip install pygame pyserial
python3 laptop_bridge.py [PORT]
```

- Tanpa `PORT`, bridge otomatis mendeteksi `/dev/ttyUSB*` atau `/dev/ttyACM*` (Linux).
- Tulis port secara eksplisit di OS lain, atau kalau ada beberapa perangkat terhubung:
  - Linux: `python3 laptop_bridge.py /dev/ttyUSB0`
  - macOS: `python3 laptop_bridge.py /dev/cu.usbserial-XXXX`
  - Windows: `python laptop_bridge.py COM3`
- Baud rate: **115200**.
- Tutup serial monitor dulu. Satu port hanya bisa dipakai satu program dalam satu waktu.

## Hardware / Wiring

| Fungsi       | GPIO ESP32 | Pin Driver |
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

- Motor A & B: differential drive. Motor C: omni wheel.
- PWM: 1 kHz, resolusi 8-bit (0–255).
- Duty maksimum: **180 / 255**.
- Deadzone: **±5** pada skala input -100..100. Nilai di dalam rentang ini dianggap 0.
- Beri daya motor dari catu daya terpisah, **bukan** dari pin 3V3/5V ESP32.
- GND ESP32 dan GND driver motor **wajib disambungkan** (ground bersama).

## Protokol Komunikasi

**Laptop → ESP32** (115200 baud, satu baris per update, diakhiri `\n`):
```
LX,LY,MASK\n
```

| Field | Rentang | Arti |
|-------|---------|------|
| `LX`  | -100..100 | Belok. Positif = kanan |
| `LY`  | -100..100 | Gas. Positif = maju |
| `MASK`| unsigned 32-bit | Bitmask tombol. Bit *n* = tombol *n* ditekan |

Contoh:
- `0,50,0` → maju setengah kecepatan, tidak belok, tidak ada tombol
- `-30,0,5` → putar kiri di tempat, tombol 0 dan 2 ditekan (`0b101`)

**ESP32 → Laptop** (echo debug setelah setiap baris berhasil diparsing):
```
LX=0 LY=50 BTN=0x0000 []
```

## Pemecahan Masalah

| Masalah | Solusi |
|---------|--------|
| `Permission denied: /dev/ttyUSB0` (Linux) | `sudo usermod -aG dialout $USER`, lalu logout dan login lagi |
| Upload macet di `Connecting....` | Tahan tombol **BOOT** di board sampai upload mulai |
| Port sibuk / bridge tidak bisa membuka port | Tutup serial monitor atau Serial Monitor Arduino IDE |
| Port tidak muncul | Install driver USB untuk board kamu (CP210x atau CH340), dan pakai kabel USB data, bukan kabel khusus charge |
| `ledcSetup` / `ledcAttachPin` not declared | Core Arduino ESP32 3.x mengubah API LEDC. Pakai core 2.x, atau ubah kode ke `ledcAttach()` / `ledcWrite(pin, duty)` |
| Motor berputar ke arah yang salah | Tukar dua kabel motornya, atau tukar definisi pin `IN`-nya di kode |

## Kontribusi — Langkah demi Langkah

### 1. Fork & Clone
Fork repo di GitHub, lalu:
```bash
git clone https://github.com/<USERNAME-KAMU>/Kontroller.git
cd Kontroller
git remote add upstream https://github.com/<PEMILIK-ASLI>/Kontroller.git
```

### 2. Buat Branch
Selalu buat branch dari `main` yang sudah terbaru:
```bash
git checkout main
git pull upstream main
git checkout -b <tipe>/<deskripsi-singkat>
# Tipe: feat, fix, refactor, docs, test, chore
# Contoh: git checkout -b feat/add-battery-monitor
```

### 3. Lakukan Perubahan
- Edit kode di `src/`. Pastikan `Kontroller.ino` tetap kompatibel dengan Arduino IDE.
- Update `platformio.ini` kalau menambah library, dan cantumkan library itu di README ini untuk pengguna Arduino IDE.
- Kalau protokol berubah, update `laptop_bridge.py` **dan** bagian Protokol di README ini dalam PR yang sama.

### 4. Tes Secara Lokal
```bash
pio run -e esp32dev        # harus build tanpa error
python3 laptop_bridge.py   # tes hardware dengan gamepad
```
Pastikan juga sketch bisa dikompilasi di Arduino IDE (**Verify**).

### 5. Commit
Ikuti [Conventional Commits](https://www.conventionalcommits.org/):
```bash
git add <file>
git commit -m "<tipe>: <ringkasan dalam bentuk perintah>"
```
Contoh:
```
feat: add battery voltage monitoring via ADC
fix: correct motor direction reversal on omni wheel
docs: update wiring table for v2 PCB
```
Tambahkan body yang menjelaskan *apa* dan *kenapa* untuk perubahan yang tidak sepele. Tandai perubahan yang merusak kompatibilitas dengan `BREAKING CHANGE:` di body.

### 6. Push & Buka PR
```bash
git push -u origin <nama-branch>
```
Buka pull request di GitHub ke `main`, jelaskan perubahan dan cara kamu mengetesnya, lalu minta review.

### 7. Review & Merge
- Tanggapi komentar review dengan push commit baru ke branch yang sama.
- Maintainer yang melakukan merge (squash atau rebase).
- Hapus branch setelah di-merge.

## Menambah Fitur Baru
1. **Desain**: jelaskan perubahan protokol atau pin di deskripsi PR.
2. **Implementasi**: ubah `src/Kontroller.ino` (+ `platformio.ini` kalau perlu).
3. **Update bridge**: ubah `laptop_bridge.py` kalau protokol berubah.
4. **Tes**: build di PlatformIO dan Arduino IDE, lalu tes di hardware.
5. **Dokumentasi**: update bagian Wiring / Protokol di README ini.

## Memperbaiki Bug
1. **Reproduksi**: simpan log serial dan output bridge.
2. **Isolasi**: cari cara paling sederhana untuk memicu bug-nya.
3. **Perbaiki**: edit file yang bersangkutan.
4. **Verifikasi**: build di kedua IDE dan pastikan tidak ada regresi di hardware.
5. **Commit**: `fix: <deskripsi>`, dan rujuk issue-nya (`Fixes #12`) kalau ada.

## Memperbarui Dokumentasi
1. Edit `README.md`.
2. Commit dengan `docs: <deskripsi>`.
3. Tidak perlu tes hardware.

## Gaya Kode
- Default Arduino/PlatformIO, indentasi 2 spasi.
- Definisi pin pakai `#define` di bagian atas `src/Kontroller.ino`.
- Fungsi: `lowerCamelCase`.
- Konstanta: `UPPER_SNAKE_CASE`.

## Lisensi
MIT — lihat [LICENSE](LICENSE).