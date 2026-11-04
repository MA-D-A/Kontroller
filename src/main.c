#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// MOTOR A - KIRI
#define ENA 25
#define IN1 26
#define IN2 27

// MOTOR B - KANAN
#define ENB 14
#define IN3 32
#define IN4 33

// MOTOR C - OMNI DEPAN
#define ENC 18
#define IN5 19
#define IN6 21

#define PWM_FREQ 1000
#define PWM_RES 8

int speedMotor = 180;

// MOTOR A
void motorA(int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } 
  else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } 
  else {
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
  } 
  else if (speed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } 
  else {
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
  } 
  else if (speed < 0) {
    digitalWrite(IN5, LOW);
    digitalWrite(IN6, HIGH);
  } 
  else {
    digitalWrite(IN5, LOW);
    digitalWrite(IN6, LOW);
  }

  ledcWrite(ENC, abs(speed));
}

// STOP
void stopMotor() {
  motorA(0);
  motorB(0);
  motorC(0);
}

// MAJU
void maju() {
  motorA(speedMotor);
  motorB(speedMotor);
  motorC(speedMotor);
}

// MUNDUR
void mundur() {
  motorA(-speedMotor);
  motorB(-speedMotor);
  motorC(-speedMotor);
}

// KIRI
void kiri() {
  motorA(-speedMotor);
  motorB(speedMotor);
  motorC(0);
}

// KANAN
void kanan() {
  motorA(speedMotor);
  motorB(-speedMotor);
  motorC(0);
}
void Mutarkanan(){
  motorA(speedMotor);
  motorB(speedMotor);
  motorC(speedMotor);
}
void Mutarkiri(){
  motorA(-speedMotor);
  motorB(-speedMotor);
  motorC(-speedMotor);
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

  SerialBT.begin("ROBOT_KRAI_3MOTOR");

  Serial.println("BLUETOOTH ROBOT 3 MOTOR");
  Serial.println("W = MAJU");
  Serial.println("A = KIRI");
  Serial.println("S = MUNDUR");
  Serial.println("D = KANAN");
  Serial.println("X = STOP");
}

void loop() {
  if (SerialBT.available()) {

    char command = toupper(SerialBT.read());

    Serial.print("Command: ");
    Serial.println(command);

    switch (command) {

      case 'F':
        maju();
        break;

      case 'L':
        kiri();
        break;

      case 'B':
        mundur();
        break;

      case 'R':
        kanan();
        break;
      case 'Q':
        Mutarkanan();
        break;
      case 'E':
        Mutarkiri();
        break;
      case 'S':
        stopMotor();
        break;

      default:
        break;
    }
  }
}