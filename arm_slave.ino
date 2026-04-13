#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVO_MIN  150
#define SERVO_MAX  600
#define SERVO_FREQ 50

#define CH_BASE     0
#define CH_SHOULDER 1
#define CH_ELBOW    2
#define CH_WRIST_P  3
#define CH_WRIST_R  4
#define CH_GRIPPER  5

int currentPos[6] = {90, 90, 90, 90, 90, 90};
String inputBuffer = "";

void setServo(uint8_t channel, int angle) {
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  pwm.setPWM(channel, 0, pulse);
}

void smoothMove(uint8_t channel, int &current, int target, int step = 3) {
  if (current < target) current = min(current + step, target);
  else if (current > target) current = max(current - step, target);
  setServo(channel, current);
}

void parseAndMove(String data) {
  int angles[6];
  int idx = 0;
  int lastComma = -1;

  for (int i = 0; i <= (int)data.length(); i++) {
    if (i == (int)data.length() || data[i] == ',') {
      if (idx < 6) {
        angles[idx++] = data.substring(lastComma + 1, i).toInt();
        lastComma = i;
      }
    }
  }

  if (idx < 6) {
    Serial.println("Incomplete packet, skipping.");
    return;
  }

  Serial.print("Received: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(angles[i]);
    if (i < 5) Serial.print(",");
  }
  Serial.println();

  smoothMove(CH_BASE,     currentPos[0], angles[0]);
  smoothMove(CH_SHOULDER, currentPos[1], angles[1]);
  smoothMove(CH_ELBOW,    currentPos[2], angles[2]);
  smoothMove(CH_WRIST_P,  currentPos[3], angles[3]);
  smoothMove(CH_WRIST_R,  currentPos[4], angles[4]);
  smoothMove(CH_GRIPPER,  currentPos[5], angles[5]);
}

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  Wire.begin();

  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);

  for (int i = 0; i < 6; i++) {
    setServo(i, 90);
    delay(200);
  }

  Serial.println("Arm ready. Waiting for glove data...");
}

void loop() {
  while (BTSerial.available()) {
    char c = BTSerial.read();
    if (c == '\n') {
      parseAndMove(inputBuffer);
      inputBuffer = "";
    } else {
      inputBuffer += c;
    }
  }
}
