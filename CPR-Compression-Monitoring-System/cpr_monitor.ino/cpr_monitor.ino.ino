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