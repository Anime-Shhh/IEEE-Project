// -------------------------------
// Motor pins and half-step table
// -------------------------------
#define IN1 11
#define IN2 10
#define IN3 9
#define IN4 8

const byte halfStepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

int currentStep = 0;
int stepDelay   = 2;
int stepsPerRev = 4000;  // Same as your motor-only code
int stepCount   = 0;
int direction   = 1;     // Start spinning forward

// -------------------------------
// Ultrasonic pins and parameters
// -------------------------------
const int TriggerPin = 2;
const int EchoPin    = 3;

// We measure every ~5 degrees:
const int degreesPerMeasurement = 5;
const int stepsBetweenMeasurements = round((float)stepsPerRev * degreesPerMeasurement / 360.0);

// -------------------------------
// Function to set motor outputs
// -------------------------------
void setStep(int step) {
  digitalWrite(IN1, halfStepSequence[step][0]);
  digitalWrite(IN2, halfStepSequence[step][1]);
  digitalWrite(IN3, halfStepSequence[step][2]);
  digitalWrite(IN4, halfStepSequence[step][3]);
}

// -------------------------------
// Move the motor one half-step
// -------------------------------
void stepMotor(int dir) {
  if (dir > 0) {
    currentStep = (currentStep + 1) % 8;
  } else {
    currentStep = (currentStep - 1 + 8) % 8;
  }
  setStep(currentStep);
  delay(stepDelay);
}

// -------------------------------
// Setup
// -------------------------------
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

  // For distance readings
  Serial.begin(9600);
}

// -------------------------------
// Main loop
// -------------------------------
void loop() {
  // Step the motor exactly like in your motor-only sketch
  stepMotor(direction);
  stepCount++;

  // If one revolution completed, flip direction and reset
  if (stepCount == stepsPerRev) {
    direction = -direction; 
    stepCount = 0;
  }

  // Take a distance measurement every ~5 degrees
  if (stepCount % stepsBetweenMeasurements == 0) {
    int angle = (stepCount * 360L) / stepsPerRev;  // 0 to 359
    int distance = CalculateDistance();
    SerialOutput(angle, distance);
  }
}

// -------------------------------
// Ultrasonic distance calculation
// -------------------------------
int CalculateDistance() {
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(1);  // (Old code used 1us, though 10us is typical)
  digitalWrite(TriggerPin, LOW);

  long duration = pulseIn(EchoPin, HIGH);
  float distance = duration * 0.017;  // Convert to cm
  return (int)distance;
}

// -------------------------------
// Print angle and distance
// -------------------------------
void SerialOutput(int angle, int distance) {
  Serial.println(String(angle) + "," + String(distance));
}
