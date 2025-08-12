# IoT-enabled-Smart-Control-Valve-for-Industrial-application

## Overview
This project presents an IoT-enabled smart control valve designed for industrial process automation. The system uses an **ESP32 microcontroller** to integrate multiple sensors and provides real-time monitoring, remote operation, and a data-driven approach to maintenance.

***

## Key Features
* **Real-time Sensing**: The system captures critical parameters such as flow, pressure, temperature, and vibration using a variety of sensors.
* **Remote Monitoring**: Sensor data is transmitted over **Wi-Fi to Google Firebase**, where a custom cloud-based dashboard facilitates remote visualization.
* **Remote Actuation**: The valve can be toggled on or off remotely through a web/mobile dashboard.
* **Anomaly Detection**: A simple "learning" approach is used to spot unusual behavior and can raise an alert if a new reading deviates from the learned baseline.
* **Proactive Maintenance**: An upcoming **ML-based anomaly detection module** is planned to forecast potential valve failures and schedule maintenance proactively.

***

## Hardware Components
The following components are used in this project:

Component and its purpose

**ESP32** - Acts as the central "brain," gathering data and communicating with the cloud.
**Flow Sensor** - Measures volumetric flow rate.
**Pressure Sensor** - Tracks line pressure.
**Temperature Sensor** - Monitors thermal conditions.
**ADXL345 Accelerometer** - Detects vibration anomalies.
**Relay Module** - Drives the solenoid valve based on commands from the ESP32.
**Solenoid Valve** - Performs fluid on/off actuation.
**DC Pump** - Creates a flow loop for testing and simulates real pipeline conditions.
**Power Supply** - Provides steady $3.3 \ V/5 \ V$ power to the ESP32 and sensors.

***

## Firmware & Data Flow
The firmware, written in the Arduino IDE, follows a straightforward loop. It first reads and filters sensor data. Next, it applies simple safety logic to decide and act, either opening or closing the valve. Finally, it uploads the readings and valve status to Firebase over Wi-Fi.

***

## Machine Learning Integration
A simple machine learning approach is used for anomaly detection. The system gathers baseline data from normal operation, learns what "good" operation looks like, and then compares new readings against this learned baseline. If something seems unusual, such as a sudden spike in vibration or pressure, the system raises an alert.

A Python script using `pandas` and `scikit-learn` is included to demonstrate a **Random Forest classifier** for this purpose. The model is trained on synthetic sensor data to predict the relay status (ON/OFF).

***

## Working Principle
1. **Startup**: The ESP32 boots up, performs a self-check on the sensors, connects to Wi-Fi, and establishes a link with Firebase.
2. **Sensing Loop**: The system continuously reads data from the flow, temperature, and vibration sensors[cite: 181, 182, 183]. [cite_start]An averaging filter is applied to smooth out spikes.
3. **Decision Making**: The system checks if any sensor reading crosses a predefined safety threshold[cite: 186, 187]. [cite_start]If a threshold is exceeded, it automatically closes the valve.
4. **Data Sharing**: The ESP32 bundles the latest readings and valve status and sends this data to Firebase, which then updates the dashboard in real-time.
5. **User Control**: Users can send commands from the dashboard to open or close the valve[cite: 195]. [cite_start]The ESP32 checks for new instructions, acts on them, and sends a confirmation back to the user.
6.**Predictive Alerts**: The system learns routine behavior and raises early-warning alerts if a reading gradually drifts from the established baseline.

This project provides a scalable foundation for advanced automation and environmental impact mitigation. Future enhancements could include cloud-based data visualization, more advanced predictive analytics, and mobile app integration for remote control.
