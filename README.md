# Arduino Projects

Projects written in C/C++ for the Arduino Uno, built and tested on a breadboard. Each folder contains one project and its `.ino` sketch.

## Projects

### 1. Railway Level Crossing System (`Railway-Level-Crossing-System`)
**Stack:** Arduino Uno, FreeRTOS, C++

- Two HC-SR04 ultrasonic sensors detect the train: one lowers the barrier, the other raises it.
- An SG90 servo moves the barrier, red/green LEDs show the crossing state and a buzzer sounds while the train is passing.
- `Railway_FreeRTOS.ino` runs three FreeRTOS tasks (train detection, barrier control, alarm) that talk through a queue. Lowering the barrier always has priority over raising it (fail-safe).
- `ProiectPATR_BarieraTren.ino` is the first, simpler version without an RTOS.

### 2. Automated Parking Barrier System (`Automated-Parking-Barrier-System`)
**Stack:** Arduino, C++, analog electronics

- Three HC-SR04 sensors: entry, exit and the parking spots.
- The servo barrier is driven by a state machine (open, wait for the car, close) built on `millis()`, without blocking delays.
- After a car enters or leaves, the 16x2 LCD shows which spots are taken.
- Capacitors on the hardware side filter noise so the sensor readings stay consistent.
- `parking_spots_monitor.ino` is a second version: two sensors watch three spots (thresholds calibrated by hand), the LCD lists the taken spots and the barrier also opens when a car leaves a spot.

### 3. Multi-Sensor Fire Detection (`Multi-Sensor-Fire-Detection-System`)
**Stack:** Arduino, C++ (classes)

- Uses a flame sensor (digital) and an LM35 temperature sensor (analog).
- The two sensors cross-check each other to cut false alarms: the fire alarm (red LED, buzzer, LCD message) starts only when the flame sensor and a high temperature agree. If only one of them reacts, the system goes into a "checking" state (blinking red LED, no siren).
- A critical temperature alone still triggers the alarm, so a fire is not missed if the flame sensor fails.
- The danger has to be confirmed on 3 consecutive readings, and the temperature is averaged over several ADC samples.

### 4. Ultrasonic Parking Sensor (`Ultrasonic-Parking-Sensor`)
**Stack:** Arduino, HC-SR04, PWM

- Works like the parking sensor on a car: an HC-SR04 measures the distance to the obstacle and the 16x2 LCD shows it in cm.
- The closer the obstacle, the brighter the LED (PWM) and the faster and higher the buzzer beeps; under 10 cm the sound is continuous.
- Everything runs on `millis()`, so measuring, beeping and the display do not block each other.

### 5. Joystick Color Game (`Joystick-Color-Game`)
**Stack:** Arduino, GPIO, ADC

- Reads the X/Y axes of an analog joystick and lights one of four LEDs for each direction.
- The joystick button turns the LEDs on and off (edge detection with software debouncing).

## Skills used
- C/C++ on microcontrollers: GPIO, ADC readings, timing with `millis()`.
- FreeRTOS: tasks, priorities, queues.
- Components: Arduino Uno, HC-SR04, SG90 servo, LM35, flame sensor, 16x2 LCD, buzzer, joystick, breadboard.
- Sensor calibration and debouncing.

## How to run
1. Clone the repository.
2. Open the `.ino` file in the Arduino IDE (for the FreeRTOS version, install the "FreeRTOS" library from the Library Manager).
3. Select the Arduino Uno board and the COM port.
4. Compile and upload.
