#!/usr/bin/env python3
"""Baca gamepad (layout Xbox) di laptop, kirim ke ESP32 via USB serial.

Kirim: "LX,LY,MASK\n"  (LX/LY -100..100, LY positif = maju, MASK bitmask tombol)
Baca:  balasan ESP32 (LX=.. LY=.. BTN=..) lalu print ke terminal.

Butuh: pip install pygame pyserial
Jalankan: python3 laptop_bridge.py [/dev/ttyUSB0]
"""
import glob
import sys
import time

import pygame
import serial

BAUD = 115200
DEAD = 0.05
PORT = sys.argv[1] if len(sys.argv) > 1 else None


def find_port():
    for pat in ("/dev/ttyUSB*", "/dev/ttyACM*"):
        hits = sorted(glob.glob(pat))
        if hits:
            return hits[0]
    return None


def main():
    pygame.init()
    pygame.joystick.init()
    if pygame.joystick.get_count() == 0:
        print("Gamepad tidak terdeteksi. Colok controller dulu.")
        return 1

    js = pygame.joystick.Joystick(0)
    js.init()
    print(f"Gamepad: {js.get_name()}  axes={js.get_numaxes()} buttons={js.get_numbuttons()}")

    port = PORT or find_port()
    if not port:
        print("Port serial ESP32 tidak ada (colok Type-C ke laptop).")
        return 1

    ser = serial.Serial(port, BAUD, timeout=0)
    print(f"Serial: {port} @ {BAUD}. Ctrl+C untuk stop.")

    try:
        while True:
            pygame.event.pump()
            lx = js.get_axis(0)
            ly = js.get_axis(1)
            if abs(lx) < DEAD:
                lx = 0.0
            if abs(ly) < DEAD:
                ly = 0.0

            x = int(lx * 100)
            y = int(-ly * 100)  # pygame: up = -1 -> maju = positif

            mask = 0
            for i in range(js.get_numbuttons()):
                if js.get_button(i):
                    mask |= (1 << i)

            ser.write(f"{x},{y},{mask}\n".encode())

            while ser.in_waiting:
                line = ser.readline().decode(errors="ignore").strip()
                if line:
                    print(line)

            time.sleep(0.02)
    except KeyboardInterrupt:
        ser.write(b"0,0,0\n")
        ser.close()
        print("\nstop")
    return 0


if __name__ == "__main__":
    sys.exit(main())
