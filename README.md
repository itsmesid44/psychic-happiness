# Gesture-Controlled Robotic Arm

A 6-DOF robotic arm controlled in real-time via IMU-based glove over Bluetooth. Built from scratch: custom SolidWorks CAD, hand-etched PCBs, and dual-Arduino firmware.

---

## Overview

The arm mirrors the operator's hand and wrist movements with ~50ms latency. An MPU6050 on the glove captures 6-axis motion data, transmits it wirelessly via HC-05 Bluetooth, and the arm-side Arduino drives 6 MG996R servos through a PCA9685 PWM driver.

No pre-built controllers. No shields. Raw IC, custom PCB, custom code.

---

## Hardware

### Arm
| Component | Role |
|-----------|------|
| Arduino UNO R3 | Arm MCU |
| PCA9685 (raw IC) | 16-channel PWM driver |
| MG996R × 6 | Joint actuation |
| 5V 10A SMPS | Servo power supply |
| HC-05 | Bluetooth slave receiver |

### Glove
| Component | Role |
|-----------|------|
| Arduino UNO R3 | Glove MCU |
| MPU6050 | 6-axis IMU (accel + gyro) |
| HC-05 | Bluetooth master transmitter |

---

## Mechanical Design

All parts designed in SolidWorks. Assembly: `Robot arm.SLDASM`

| File | Part |
|------|------|
| `Base servo mount.SLDPRT` | Rotational base |
| `Lower arm left/right.SLDPRT` | Lower arm linkage pair |
| `Center arm.SLDPRT` | Mid-section arm |
| `Center arm bearing guide.SLDPRT` | Bearing race guide |
| `Center arm bearing guide clamp.SLDPRT` | Clamp for bearing guide |
| `Upper arm left/right.SLDPRT` | Upper arm linkage pair |
| `Upper arm servo mount.SLDPRT` | Servo bracket at elbow |
| `Bearing holder.SLDPRT` | Joint bearing housing |
| `Lower arm bushing.SLDPRT` | Friction bushing |

---

## Electronics

Custom PCBs designed in EasyEDA.

- `armcontrollergerber.zip` — Arm PCB (PCA9685 IC + servo headers + power terminal)
- `glovecontrollergerber.zip` — Glove PCB (MPU6050 + HC-05 + UNO headers)

### Wiring Summary

**Glove UNO → MPU6050**
- A4 → SDA, A5 → SCL, 5V → VCC, GND → GND

**Arm UNO → PCA9685**
- A4 → SDA, A5 → SCL, 5V → VDD, GND → VSS
- A0–A3 address pins → GND (I2C address 0x40)
- #OE → GND (outputs always enabled)

**PCA9685 CH0–CH5 → MG996R signal pins**

**SMPS 5V → all servo VCC rails (common GND)**

**Both UNOs: HC-05 on D2/D3 via SoftwareSerial**
- TX → RX with 1kΩ/2kΩ voltage divider on RX line

---

## Firmware

Both sketches in `code.zip`.

### Glove (Master)
- Reads 6-axis motion via `mpu.getMotion6()`
- Maps accel/gyro axes to 0–180° servo ranges
- Sends CSV packet `s1,s2,s3,s4,s5,s6\n` at 20Hz over Bluetooth

### Arm (Slave)
- Parses incoming CSV over SoftwareSerial
- Drives 6 servos via PCA9685 with 3°/packet smoothing
- Centers all servos to 90° on startup

### Libraries Required
```
MPU6050 (Electronic Cats)
Adafruit PWM Servo Driver Library
Wire (built-in)
SoftwareSerial (built-in)
```

### HC-05 Pairing
Set glove HC-05 as master (`AT+ROLE=1`), arm HC-05 as slave (`AT+ROLE=0`). Pair them at matching baud (9600).

---

## Degrees of Freedom

| Channel | Joint | Axis |
|---------|-------|------|
| CH0 | Base | Rotation (ax) |
| CH1 | Shoulder | Pitch (ay) |
| CH2 | Elbow | Pitch (az) |
| CH3 | Wrist Pitch | (gx) |
| CH4 | Wrist Roll | (gy) |
| CH5 | Gripper | (gz) |

---

## Build Notes

- Servo power must come from SMPS, not Arduino 5V — MG996R draws up to 2.5A stall per servo
- Common GND across Arduino, PCA9685, and SMPS is mandatory
- Tune `SERVO_MIN`/`SERVO_MAX` (150/600) if servos don't reach full range — varies by batch
- Increase `smoothMove` step value (default 3) for faster response, decrease for smoother motion
- Add 470–1000µF capacitor across PCA9685 V+ if servos jitter under load

---

## Repo Structure

```
psychic-happiness/
├── *.SLDPRT              # SolidWorks part files
├── Robot arm.SLDASM      # Full assembly
├── armcontrollergerber.zip
├── glovecontrollergerber.zip
└── code.zip              # glove_master.ino + arm_slave.ino
```

---

## Built By
Part of the Stasis program.
