#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3);
MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("MPU6050 ready.");
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  int s1 = map(constrain(ax, -17000, 17000), -17000, 17000, 0, 180);
  int s2 = map(constrain(ay, -17000, 17000), -17000, 17000, 0, 180);
  int s3 = map(constrain(az, -17000, 17000), -17000, 17000, 0, 180);
  int s4 = map(constrain(gx, -17000, 17000), -17000, 17000, 0, 180);
  int s5 = map(constrain(gy, -17000, 17000), -17000, 17000, 0, 180);
  int s6 = map(constrain(gz, -17000, 17000), -17000, 17000, 0, 180);

  BTSerial.print(s1); BTSerial.print(",");
  BTSerial.print(s2); BTSerial.print(",");
  BTSerial.print(s3); BTSerial.print(",");
  BTSerial.print(s4); BTSerial.print(",");
  BTSerial.print(s5); BTSerial.print(",");
  BTSerial.println(s6);

  Serial.print("Sending: ");
  Serial.print(s1); Serial.print(",");
  Serial.print(s2); Serial.print(",");
  Serial.print(s3); Serial.print(",");
  Serial.print(s4); Serial.print(",");
  Serial.print(s5); Serial.print(",");
  Serial.println(s6);

  delay(50);
}
