// Combined Sonar and Sound Sensor for Naval Hackathon
// -------------------------------

// Motor pins and half-step table
#define IN1 11
#define IN2 10
#define IN3 9
#define IN4 8

const byte halfStepSequence[8][4] = {
  {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
  {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1}
};

int currentStep = 0;
int stepsPerRev = 4000; // Number of motor step calls per revolution
int stepCount = 0;
int direction = 1;

// Ultrasonic pins and parameters
const int TriggerPin = 2;
const int EchoPin = 3;
const int degreesPerMeasurement = 5;
const int stepsBetweenMeasurements = round((float)stepsPerRev * degreesPerMeasurement / 360.0);

// Sound sensor pins and parameters
int soundPin = A0;
int soundValue = 0;
int threshold = 470; // Adjust based on ambient noise
int peakCount = 0;
unsigned long lastPeakTime = 0;
unsigned long windowStart = 0;
bool inWindow = false;
unsigned long peakTimes[5] = {0, 0, 0, 0, 0};

// Stealth mode parameters
const int stealthPin = 7; // Output for stealth mode indicator (e.g., buzzer/LED)
bool inStealthMode = false;
unsigned long stealthStartTime = 0;
const unsigned long stealthDuration = 10000; // 10 seconds

// Timing parameters
const unsigned long stepInterval = 2; // ms between motor steps
const unsigned long soundInterval = 5; // ms between sound readings
unsigned long lastStepTime = 0;
unsigned long lastSoundTime = 0;

// Function to set motor outputs
void setStep(int step) {
  digitalWrite(IN1, halfStepSequence[step][0]);
  digitalWrite(IN2, halfStepSequence[step][1]);
  digitalWrite(IN3, halfStepSequence[step][2]);
  digitalWrite(IN4, halfStepSequence[step][3]);
}

// Move the motor one half-step (no delay)
void stepMotor(int dir) {
  if (dir > 0) {
    currentStep = (currentStep + 1) % 8; // As per new code
  } else {
    currentStep = (currentStep - 1 + 8) % 8;
  }
  setStep(currentStep);
}

// Setup
void setup() {
  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  setStep(0);

  // Ultrasonic pins
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  // Stealth mode pin
  pinMode(stealthPin, OUTPUT);
  digitalWrite(stealthPin, LOW);

  // Serial for GUI and debug
  Serial.begin(9600); // Match Processing GUI baud rate
}

// Main loop
void loop() {
  unsigned long currentTime = millis();

  // Sound reading and processing (every 5 ms)
  if (currentTime - lastSoundTime >= soundInterval) {
    soundValue = analogRead(soundPin);
    lastSoundTime = currentTime;

    // Peak detection
    if (soundValue > threshold && (currentTime - lastPeakTime > 50)) {
      lastPeakTime = currentTime;
      if (!inWindow) {
        windowStart = currentTime;
        inWindow = true;
        peakCount = 1;
        peakTimes[0] = currentTime;
      } else if (peakCount < 5) {
        peakTimes[peakCount] = currentTime;
        peakCount++;
      }
    }

    // Check for enemy detection (5 peaks within 6 seconds)
    if (inWindow && (currentTime - windowStart >= 6000 || peakCount >= 5)) {
      if (peakCount >= 5 && (peakTimes[4] - peakTimes[0] <= 6000)) {
        inStealthMode = true;
        stealthStartTime = currentTime;
        digitalWrite(stealthPin, HIGH);
        Serial.println("Stealth Mode Activated");
      }
      inWindow = false;
      peakCount = 0;
    }
  }

  // Handle stealth mode
  if (inStealthMode) {
    // Check if 10 seconds have elapsed
    if (currentTime - stealthStartTime >= stealthDuration) {
      // Exit stealth if no recent enemy detection (simplified check)
      if (currentTime - lastPeakTime > 6000) { // No peaks in last 6 seconds
        inStealthMode = false;
        digitalWrite(stealthPin, LOW);
        Serial.println("Stealth Mode Deactivated");
      }
    }
  } else {
    // Normal operation: step motor every 2 ms
    if (currentTime - lastStepTime >= stepInterval) {
      stepMotor(direction);
      lastStepTime = currentTime;
      stepCount++;

      // Reverse direction at revolution end
      if (stepCount >= stepsPerRev) {
        direction = -direction;
        stepCount = 0;
      }

      // Take sonar measurement every 56 steps
      if (stepCount % stepsBetweenMeasurements == 0) {
        int angle = (stepCount * 360L) / stepsPerRev;
        int distance = CalculateDistance();
        SerialOutput(angle, distance);
      }
    }
  }
}

// Ultrasonic distance calculation
int CalculateDistance() {
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  long duration = pulseIn(EchoPin, HIGH);
  float distance = duration * 0.017; // cm
  return (int)distance;
}

// Print angle and distance
void SerialOutput(int angle, int distance) {
  Serial.println(String(angle) + "," + String(distance));
}