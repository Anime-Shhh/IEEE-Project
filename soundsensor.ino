// Enhanced Sound Sensor Graphing with Threshold
int soundPin = A0;
int soundValue = 0;
int threshold = 490;  // Adjust based on your ambient noise (test to find a good value)
int repeat = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  soundValue = analogRead(soundPin);
  Serial.println(soundValue);  // Plot all values

  if (soundValue > threshold) {
    Serial.print("Loud Sound Detected: ");  // Optional text for Serial Monitor
    Serial.println(soundValue);
    repeat++;
  }

  if(repeat > 6){
    Serial.println("This is a ai submarine");
  }
  delay(100);  // 20 Hz sampling rate
}
