/**
 * Robot KRAI 3 roda omni - ESP32 + Bluepad32
 *   LS : maju/mundur + geser kanan/kiri
 *   RS : putar (kanan = putar positif)
 */
#include <Bluepad32.h>

// ----- Pin motor {EN, IN1, IN2} intinya pin motor driver ke  esp 
struct Motor { uint8_t en, in1, in2; };
const Motor motors[3] = {
  {25, 26, 27},   // A
  {19, 16, 17},   // B
  {18, 13, 23},   // C 
};

const int   MAX_SPEED = 200;    // batas PWM (0-255)
const float DEADZONE  = 0.08f;  // 8%

// ----- Matriks kinematik: {kanan, maju, putar} per motor -----
const float K[3][3] = {
  { 0.5f,  0.866f, 1.0f },   // A
  { 0.5f, -0.866f, 1.0f },   // B
  {-1.0f,  0.0f,   1.0f },   // C
};

ControllerPtr gamepad = nullptr;

// kode buat nge set motor(penghubung bacaan dari kontroller ke motor drivernya)
void setMotor(const Motor& m, float v) {
  digitalWrite(m.in1, v > 0);
  digitalWrite(m.in2, v < 0);
  analogWrite(m.en, (int)(fabsf(v) * MAX_SPEED));
}

void stopAll() {
  for (auto& m : motors) setMotor(m, 0);
}

// kode kinematika omni wheel tiga rodanya intinya yang ngubah  array kinematikanya jadi arah gerak motornya
void drive(float vx, float vy, float w) {
  float out[3];
  float maxMag = 1.0f;
  for (int i = 0; i < 3; i++) {
    out[i] = K[i][0] * vx + K[i][1] * vy + K[i][2] * w;
    maxMag = fmaxf(maxMag, fabsf(out[i]));
  }
  // Skala bersama agar arah gerak tetap terjaga saat saturasi (normalisasi nilai misal kalau nanti hasilnya ada yang 1.8 di normalisasi jadi 1)
  for (int i = 0; i < 3; i++) setMotor(motors[i], out[i] / maxMag);
}

// Axis Bluepad32  mengubah nilai mentah  blupad32 jadi 1.1 sampe   
float axis(int raw) {
  float v = raw / 512.0f;
  return fabsf(v) < DEADZONE ? 0.0f : v;
}

void onConnected(ControllerPtr c) {
  if (gamepad == nullptr) {
    gamepad = c;
    Serial.println("Gamepad terhubung");
  } else {
    c->disconnect(); 
  }
}

void onDisconnected(ControllerPtr c) {
  if (c == gamepad) {
    gamepad = nullptr;
    stopAll();   // failsafe
    Serial.println("Gamepad terputus");
  }
}

void setup() {
  Serial.begin(115200);

  for (auto& m : motors) {
    pinMode(m.in1, OUTPUT);
    pinMode(m.in2, OUTPUT);
  }
  stopAll();

  BP32.setup(&onConnected, &onDisconnected);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
  BP32.enableNewBluetoothConnections(true);
}

void loop() {
  if (BP32.update() && gamepad && gamepad->isConnected()) {
    float vx =  axis(gamepad->axisX());    // LS kanan
    float vy = -axis(gamepad->axisY());    // LS atas = maju
    float w  =  axis(gamepad->axisRX());   // RS kanan = putar
    drive(vx, vy, w);

    // Cetak nilai stik tiap 100 ms ya supaya bisa debug nanti  arah kontrollernya udah bener belumn nyambung ke esp32 nya
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 100) {
      lastPrint = millis();
      Serial.printf("LX=%.2f LY=%.2f RX=%.2f\n", vx, vy, w);
    }
  }
  delay(10);
}