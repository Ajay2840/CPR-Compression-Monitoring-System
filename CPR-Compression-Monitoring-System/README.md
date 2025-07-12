# CPR Compression Monitoring System 🚑

An IoT-based system using **Arduino Uno**, **Ultrasonic sensor**, and **Bluetooth (HC-05)** to monitor chest compressions during CPR training. Visual feedback is provided in real-time using a custom-built mobile app created with **MIT App Inventor**.

## Features
- Measures compression **depth** and **accuracy**
- Tracks **total count** and **CPM (compressions per minute)**
- 60-second session timer
- Bluetooth communication with Android app

## Hardware Used
- Arduino Uno
- HC-05 Bluetooth Module
- Ultrasonic Sensor (HC-SR04)
- 5V power supply / USB
- Android Phone with MIT App Inventor App
- Breadboard
- Jumper Wires

## 🛠️ Circuit Overview

### 🔹 Ultrasonic Sensor (HC-SR04)
- **VCC** → 5V  
- **GND** → GND  
- **Trig** → Digital Pin **9**  
- **Echo** → Digital Pin **10**

---

### 🔹 Bluetooth Module (HC-05)
- **VCC** → 5V  
- **GND** → GND  
- **TX** → Arduino **RX** (Pin 0)  
- **RX** → Arduino **TX** (Pin 1) via voltage divider *(to reduce 5V to ~3.3V)*

> ⚠️ **Note:** Do **not connect HC-05 RX directly** to Arduino TX without a voltage divider. Use two resistors (e.g., 1kΩ + 2kΩ) to safely step down 5V to ~3.3V.

---

### 🔹 Arduino Uno
- Powers both sensors via 5V and GND  
- Receives compression data from HC-SR04  
- Sends processed CPR data over Bluetooth to a mobile app

## 🧾 Code Overview

```cpp

#define TRIG_PIN 9
#define ECHO_PIN 10

long duration;
float distance;
float lowerThreshold = 6.0;
float upperThreshold = 10.0;
float idealDistance = 6.0;

bool wasReleased = true;
bool timerStarted = false;

unsigned long startTime = 0;
unsigned long lastSecondMark = 0;

int sessionCount = 0;
int totalCount = 0;
int timeLeft = 60;
int cpm = 0;
bool sessionEnded = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // Trigger ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure distance
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration * 0.0343) / 2;

  // Calculate accuracy
  int accuracy = 100 - (int)((abs(distance - idealDistance) / idealDistance) * 100.0);
  if (accuracy < 0) accuracy = 0;

  unsigned long currentTime = millis();

  // Detect release
  if (distance > upperThreshold) {
    wasReleased = true;
  }

  // Detect compression
  if (distance <= lowerThreshold && wasReleased) {
    wasReleased = false;

    // Start timer on first compression
    if (!timerStarted) {
      timerStarted = true;
      sessionEnded = false;
      startTime = currentTime;
      lastSecondMark = currentTime;
      sessionCount = 0;
      timeLeft = 60;
    }

    sessionCount++;
    totalCount++;
    cpm = sessionCount;  // CPM updates with every compression
  }

  // Timer logic
  if (timerStarted) {
    unsigned long elapsed = currentTime - startTime;

    // Countdown
    if (elapsed - (lastSecondMark - startTime) >= 1000 && timeLeft > 0) {
      timeLeft = 60 - (elapsed / 1000);
      lastSecondMark = currentTime;
    }

    // End of 60 seconds
    if (elapsed >= 60000 && !sessionEnded) {
      sessionEnded = true;
      timerStarted = false;  // Stop timer
    }

    // Output all in one line
    Serial.print(distance); Serial.print(";");
    Serial.print(accuracy); Serial.print(";");
    Serial.print(totalCount); Serial.print(";");
    Serial.print(timeLeft); Serial.print(";");
    Serial.print(cpm); Serial.println(";");

    // Reset all after a short delay (after session ends)
    if (sessionEnded && elapsed >= 65000) {
      cpm = 0;
      sessionCount = 0;
      timeLeft = 60;
      sessionEnded = false;
      delay(1000);  // Small pause before accepting next session
    }
  }

  delay(100);
}
```

## Mobile App
- Built using **MIT App Inventor**
- Displays live CPR stats
- Optional voice feedback

## How It Works
1. Place sensor above the compression point.
2. Begin CPR — the system detects compressions in real-time.
3. App displays live stats via Bluetooth.
4. After 60 seconds, session ends and data resets.


## 📌 Authors

- Pulagam Ajay Kumar Reddy (B22EC007)


## 📚 References

- [Arduino Tutorials](https://www.arduino.cc/en/Tutorial/HomePage)
- [DHT11 Datasheet](https://components101.com/sensors/dht11-temperature-sensor)
- [HC-05 Module Guide](https://lastminuteengineers.com/bluetooth-module-arduino-tutorial)


## License
This project is open-source and free to use under the MIT License.

