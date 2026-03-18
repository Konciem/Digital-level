# Electronic Digital Level

A digital level project based on the Raspberry Pi Pico (RP2040) microcontroller. The device measures the inclination angle in two axes (X and Y) in real-time and displays the results on an OLED screen.

The entire system is powered by a Li-Ion cell with charging and battery status monitoring capabilities, and the electronics are enclosed in a 3D-printed case.

## Main Features

* **Real-time Measurement:** Displaying vertical and horizontal inclination angles.
* **Level Signaling (0°):**
    * Acoustic signal (Buzzer).
    * Haptic feedback (Vibration motor).
* **Calibration:** Angle zeroing (taring) function via a physical button.
* **Offset:** Ability to set a custom reference point.
* **Power System:** Built-in USB charging and **battery percentage readout** displayed on the screen.
* **Interface:** Operated using 3 physical buttons.

## Component List

### Microcontroller and Sensors
* **Raspberry Pi Pico** (RP2040) or Pico W.
* **MPU6050** – 3-axis accelerometer and 3-axis gyroscope (I2C communication, 3.3V).

### Display and Signaling
* **OLED 1.3" (SH1106)** or 0.96" (SSD1306) – I2C interface.
* **Active Buzzer** (3.3V – 5V, e.g., KY-012).
* **3V Vibration Motor** (coin type or from a phone) controlled via an **NPN transistor (e.g., 2N2222)** + 1kΩ base resistor.

### Power and Battery Monitoring
* **Li-Ion 3.7V Cell** (e.g., 18650 or a flat Li-Po cell).
* **TP4056 Charging Module** (with USB-C/MicroUSB protection).
* **Step-Up Converter** (boosting voltage to 5V for Pico VSYS or stable 3.3V).
* **Slide Switch** (to disconnect battery power).
* **Voltage Divider (for battery measurement):**
    * 2x Resistors (e.g., 10kΩ and 10kΩ or 100kΩ and 100kΩ) connected to an ADC pin for safe measurement of voltage > 3.3V.

### Control and Others
* 4x **Tact Switch Buttons** (6x6 mm) – (Menu / Calibration / Reset / Battery Status).
* Resistors.
* 3D-printed enclosure.

## Wiring Diagram (Pinout)

| Component | Pico Pin (Suggested) |
| :--- | :--- |
| **MPU6050 SDA** | GP4 (I2C0 SDA) |
| **MPU6050 SCL** | GP5 (I2C0 SCL) |
| **OLED SDA** | GP4 (I2C0 SDA) |
| **OLED SCL** | GP5 (I2C0 SCL) |
| **Buzzer** | GP15 |
| **Vibration (Base)** | GP14 |
| **Button 1** | GP16 |
| **Button 2** | GP17 |
| **Button 3** | GP18 |
| **Battery Measurement**| GP26 (ADC0) |

## 3D Enclosure

The enclosure was designed to house all components and provide a stable base for accurate measurements.
