# Development of a Device for Estimation of Ambient Temperature by Measuring Ultrasonic Propagation Velocity in Air


### Authors

[<u>Saransh</u>](https://saranshvas.github.io/)<sup>1,2</sup>, [<u>Nitin Dhiman</u>](https://www.researchgate.net/profile/Nitin-Dhiman)<sup>2,3</sup>, and [<u>P.K. Dubey</u>](https://www.researchgate.net/profile/P-K-Dubey)<sup>2,3</sup>

<p align="left">
  <sup>1</sup> <i>Department of Electronics Engineering, J.C. Bose University of Science and Technology, YMCA, Faridabad, India</i><br>
  <sup>2</sup> <i>Pressure, Vacuum and Ultrasonic Metrology, Division of Physico-Mechanical Metrology, CSIR-National Physical Laboratory, New Delhi, India</i><br>
  <sup>3</sup> <i>Academy of Scientific and Innovative Research (AcSIR), Ghaziabad, India</i>
</p>


## Overview

The system measures ultrasonic time-of-flight (TOF) with an HC-SR04 sensor and an ATmega16A microcontroller, calculates the ultrasonic propagation velocity in air, and estimates temperature with high precision (±0.4 °C). Results are displayed in real-time via a 16x2 LCD, demonstrating repeatable and reliable measurements suitable for laboratory and industrial applications.

🎉 Accepted: In proceedings of International Conference on Ultrasonics and Materials Science for Advanced Technology (ICUMSAT-2025)

📄 Paper : https://www.researchgate.net/publication/400065300_Development_of_a_Device_for_Estimation_of_Ambient_Temperature_by_Measuring_Ultrasonic_Propagation_Velocity_in_Air


## Quick Start

1. Compile the code using CodeVisionAVR for ATmega16A.
2. Connect HC-SR04:
   - TRIG → PD0
   - ECHO → PD1
3. Interface the 16×2 LCD to PORTC (default CodeVisionAVR LCD configuration).
4. Place a flat reflector at 100 cm from the sensor.
5. Power the system and observe velocity and temperature on the LCD.


## System Architecture and Hardware Components

The device is built using a compact, embedded setup optimized for high-precision ultrasonic temperature measurement. Each component is carefully selected to ensure accuracy, repeatability, and real-time performance.

| Component           | Model / Specs                 | Purpose |
|--------------------|-------------------------------|---------|
| Microcontroller     | ATmega16A                     | Timing, TOF calculation, and system control |
| Ultrasonic Sensor   | HC-SR04                        | Pulse emission & echo detection |
| Acoustic Reflector  | Square Block, e.g., 30×30×2 cm | Fixed propagation path for velocity measurement |
| Display             | 16x2 LCD                       | Real-time velocity & temperature display |
| Programmer          | USBasp                         | Microcontroller flashing and programming |
| Oscilloscope        | Generic / Tektronix            | Signal monitoring and waveform verification |
| Spectrum Analyzer   | Generic                        | Frequency-domain analysis of ultrasonic signals |
| Digital Multimeter  | DMM (Multimeter)               | Voltage, current, and continuity measurements |
| Supporting Hardware | PCB, jumper wires              | Signal conditioning, mechanical integration, and stable interfacing |


## How it works

### 1. Ultrasonic Pulse Emission and TOF Measurement
1. The HC-SR04 emits a 10 µs ultrasonic pulse toward a fixed reflector.

2. The ATmega16A measures the round-trip time 𝑡 digitally using a hardware timer.

3. The ultrasonic velocity 𝑐 is computed using:
```
    c = 2d / t
```
​
where 𝑑 = 1m is the reflector distance.

4. Invalid measurements caused by missed echoes or spurious reflections are discarded.
```c
void send_trigger() {
    TRIG = 1;
    delay_us(10);
    TRIG = 0;
}

while (ECHO == 0);
TCNT1 = 0;

while (ECHO == 1 && count < 60000) {
    count = TCNT1;
}
```
### 2. Multi-Sample Averaging and Temporal Stabilization

1. To enhance measurement reliability, multi-sample averaging is applied over 200 valid velocity readings.

2. Only valid measurements are accumulated to compute the average ultrasonic velocity.

3. Temporal stabilization is applied using exponential smoothing:

   𝑣<sub>smoothed</sub> = 0.9⋅𝑣<sub>old</sub> + 0.1⋅𝑣<sub>new</sub>
```c
float sum = 0;
int valid_readings = 0;
for (int i = 0; i < 200; i++) {
    send_trigger();
    time = measure_pulse();
    if (time > 200 && time < 60000) {
        sum += velocity;
        valid_readings++;
    }
}
velocity = sum / valid_readings;

float smooth_value(float new_value, float old_value) {
    return (0.9 * old_value) + (0.1 * new_value);
}
```
### 3. Temperature Estimation

   The ambient temperature 𝑇 is estimated from the smoothed ultrasonic velocity using the linear acoustic model:
```
c = 331.4 + 0.606 T
```
​
where 331.4 m/s is the speed of sound at 0 °C, and 0.606 m/s per °C is the temperature coefficient.

### 4. Display and Update

1. Real-time temperature and velocity values are displayed on a 16x2 LCD.

2. A fixed inter-sample delay (10 ms) prevents interference from residual echoes.

3. This implementation ensures stable, repeatable, and high-precision measurements (±0.4 °C).


## Understanding the Evaluation Metrics

To assess the performance of our ultrasonic temperature measurement device, we compared it against the National Physical Laboratory (NPL) calibrated temperature standard. We focus on two key aspects of measurement quality: accuracy and stability.

### 1. Measurement Accuracy

**Definition:** The difference between the temperature measured by our device and the NPL reference.

**Goal:** Minimize the deviation to ensure highly precise readings.

**Interpretation:** Smaller deviation indicates higher accuracy and closer adherence to the national standard.

### 2. Measurement Stability

**Definition:** The consistency of repeated temperature measurements over time under identical conditions.

**Goal:** Achieve minimal fluctuation between successive readings.

**Interpretation:** Lower variance indicates better stability and reliability of the device.

## Key Takeaways

1.  **High Accuracy:** Our device consistently measures temperature within ±0.4 °C of the NPL standard, demonstrating precise calibration.

2.  **High Stability:** Multi-sample averaging and temporal smoothing ensure repeatable, reliable readings.

3.  **Overall Performance:** The ideal device combines both high accuracy and stability, providing a trustworthy, contactless, and low-cost temperature measurement solution.


## Available Methods

1. Contact sensors – Thermistors or RTDs; require physical contact, slower response in air.

2. Single TOF ultrasonic – Existing ultrasonic approaches; rely on single-sample TOF, higher uncertainty.

3. Wireless/IoT sensors – Distributed monitoring using wireless sensors; needs calibration and network setup.

4. Infrared/optical – Non-contact IR/optical sensors; sensitive to emissivity and environmental factors.


## Key Metrics to Compare

When evaluating your ultrasonic temperature measurement device against a calibrated reference (e.g., NPL standard), focus on:

1. **Measurement Accuracy:** Deviation from the reference temperature; smaller is better.

2. **Measurement Stability:** Consistency of repeated readings; lower variation is better.

3. **Response Time:** Time to reach a stable reading; faster is better.

4. **Practical Reproducibility:** Ability to reproduce results when adjusting parameters like reflector distance, sample size, or smoothing factor.


## Citation
```bib
@inproceedings{inproceedings,
author = {., Saransh and Dhiman, Nitin and Dubey, P. K.},
year = {2026},
month = {01},
pages = {},
title = {Development of a Device for Estimation of Ambient Temperature by Measuring Ultrasonic Propagation Velocity in Air}
}
```
