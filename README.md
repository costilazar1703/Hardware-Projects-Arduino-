# Embedded Systems & Arduino Projects Portfolio 

A comprehensive collection of hardware-software integration projects developed in C/C++ for the **Arduino Uno** ecosystem. This repository demonstrates practical applications of microcontroller programming, ranging from basic analog hardware interactions to complex real-time multitasking systems.

## 📂 Projects Directory

The repository is organized into individual projects, ranked from most complex to fundamental hardware interactions:

### 1. Railway Level Crossing System (`/Railway-Level-Crossing-System`)
* **Tech Stack:** Arduino Uno, FreeRTOS, C++
* **Description:** A real-time safety simulation using **FreeRTOS** to manage concurrent tasks. It seamlessly processes data from **HC-SR04 ultrasonic sensors** to detect approaching trains while simultaneously driving **SG90 servo motors** for the barrier and triggering acoustic alarms. Features strict fail-safe event prioritization to eliminate latency.

### 2. Automated Parking Barrier System (`/Automated-Parking-Barrier-System`)
* **Tech Stack:** Arduino, Analog Electronics
* **Description:** A complete logic control system for parking access. Integrates ultrasonic distance measurement with servomotor state machines to automate entry and exit, complete with hardware-level noise-filtering capacitors to ensure consistent sensor readings.

### 3. Multi-Sensor Fire Detection (`/Multi-Sensor-Fire-Detection-System`)
* **Tech Stack:** Arduino, Sensor Fusion
* **Description:** A hybrid environmental monitoring system. It cross-validates data from multiple analog sources (flame and temperature sensors) to eliminate false positives and trigger multi-stage alarm protocols based on dynamic thresholds.

### 4. Ultrasonic Parking Sensor (`/Ultrasonic-Parking-Sensor`)
* **Tech Stack:** Arduino, Distance Calculation
* **Description:** A fundamental driver-assistance simulation. Uses HC-SR04 acoustic wave reflections to calculate real-time distance and map it to visual/audio warning frequencies.

### 5. Joystick Color Game (`/Joystick-Color-Game`)
* **Tech Stack:** Arduino, GPIO Polling
* **Description:** A hardware interaction project mapping physical analog joystick inputs (X/Y axes) to dynamic RGB LED color mixing and state changes.

## 🛠️ Core Skills & Hardware Demonstrated
* **Microcontroller Programming:** C/C++ (Arduino IDE), GPIO manipulation, memory optimization.
* **Real-Time Operating Systems:** Task scheduling, synchronization, and deadlock prevention using FreeRTOS.
* **Hardware Components:** Arduino Uno, HC-SR04 (Ultrasonic), SG90 (Servos), analog sensors, breadboarding.
* **Electronics:** Circuit design, signal calibration, and hardware debouncing.

## 💻 How to Use
Each folder contains its own `.ino` source file. To run any project:
1. Clone the repository.
2. Open the desired `.ino` file in the [Arduino IDE](https://www.arduino.cc/en/software).
3. Connect your Arduino Uno board and verify the COM port.
4. Compile and upload the code to the microcontroller.
