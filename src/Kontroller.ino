#include <Arduino.h>

// Kontrol via laptop: gamepad fatech (layout Xbox) dibaca laptop,
// dikirim ke ESP32 lewat USB serial sebagai "LX,LY,MASK\n".
//   LX/LY : -100..100  (LY positif = maju)
//   MASK  : bitmask tombol, bit i = tombol i

// MOTOR A - KIRI
#define ENA 14
#define IN1 26
#define IN2 27

// MOTOR B - KANAN
#define ENB 25
#define IN3 32
#define IN4 33

// MOTOR C - OMNI DEPAN
#define ENC 18
#define IN5 19
#define IN6 21

#define PWM_FREQ 1000
#define PWM_RES 8

#define MAX_SPEED 180
#define DEADZONE 5

// MOTOR A
void motorA(int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  ledcWrite(ENA, abs(speed));
}

// MOTOR B
void motorB(int speed) {
  if (speed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (speed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  ledcWrite(ENB, abs(speed));
}

// MOTOR C
void motorC(int speed) {
  if (speed > 0) {
    digitalWrite(IN5, HIGH);
    digitalWrite(IN6, LOW);
  } else if (speed < 0) {
    digitalWrite(IN5, LOW);
    digitalWrite(IN6, HIGH);
  } else {
    digitalWrite(IN5, LOW);
    digitalWrite(IN6, LOW);
  }
  ledcWrite(ENC, abs(speed));
}

void stopMotor() {
  motorA(0);
  motorB(0);
  motorC(0);
}

// -100..100 -> -MAX_SPEED..MAX_SPEED
int scaleSpeed(int v) {
  return (v * MAX_SPEED) / 100;
}

void applyControl(int lx, int ly, unsigned long mask) {
  if (abs(lx) < DEADZONE) lx = 0;
  if (abs(ly) < DEADZONE) ly = 0;

  Serial.printf("LX=%d LY=%d BTN=0x%04lX [", lx, ly, mask);
  bool first = true;
  for (int i = 0; i < 16; i++) {
    if (mask & (1UL << i)) {
      if (!first) Serial.print(",");
      Serial.print(i);
      first = false;
    }
  }
  Serial.println("]");

  // arcade drive: maju/mundur = LY, putar = LX
  int fwd = ly;
  int turn = lx;
  int left = constrain(fwd + turn, -100, 100);
  int right = constrain(fwd - turn, -100, 100);

  motorA(scaleSpeed(left));
  motorB(scaleSpeed(right));
  motorC(scaleSpeed(fwd));
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);

  ledcAttach(ENA, PWM_FREQ, PWM_RES);
  ledcAttach(ENB, PWM_FREQ, PWM_RES);
  ledcAttach(ENC, PWM_FREQ, PWM_RES);

  stopMotor();

  Serial.println("READY: kirim LX,LY,MASK (contoh 0,50,0)");
}

void loop() {
  static char buf[64];
  static uint8_t idx = 0;
  static unsigned long lastHb = 0;

  unsigned long now = millis();
  if (now - lastHb >= 2000) {
    lastHb = now;
    Serial.printf("HB t=%lu s\n", now / 1000);
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (idx > 0) {
        buf[idx] = '\0';
        int lx = 0, ly = 0;
        unsigned long mask = 0;
        if (sscanf(buf, "%d,%d,%lu", &lx, &ly, &mask) == 3) {
          applyControl(lx, ly, mask);
        }
      }
      idx = 0;
    } else if (idx < sizeof(buf) - 1) {
      buf[idx++] = c;
    } else {
      idx = 0;  // overflow, buang
    }
  }
}
