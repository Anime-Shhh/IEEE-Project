// Sound Sensor with Enemy Ship Detection and Frequency
int soundPin = A0;
int soundValue = 0;
int threshold = 470;  // Adjust based on your ambient noise
int peakCount = 0;    // Count peaks above threshold
unsigned long lastPeakTime = 0;  // Time of last peak
unsigned long windowStart = 0;   // Start of 6-second window
bool inWindow = false;           // Flag for active 6-second window
unsigned long peakTimes[5] = {0, 0, 0, 0, 0};  // Store times of 5 peaks

void setup() {
  Serial.begin(115200);  // Faster baud for smoother graph
}

void loop() {
  // Average 5 readings for smoother graph
  long sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(soundPin);
    delay(2);  // 10ms total for 5 samples
  }
  soundValue = sum / 5;  // Average value
  Serial.println(soundValue);  // Send to Serial Plotter

  unsigned long currentTime = millis();

  // Check for threshold break
  if (soundValue > threshold) {
    if (currentTime - lastPeakTime > 50) {  // Debounce: min 50ms between peaks
      lastPeakTime = currentTime;

      // Start 6-second window on first peak if not already started
      if (!inWindow) {
        windowStart = currentTime;
        inWindow = true;
        peakCount = 1;
        peakTimes[0] = currentTime;
      } 
      // Add subsequent peaks within window
      else if (peakCount < 5) {
        peakTimes[peakCount] = currentTime;
        peakCount++;
      }
    }
  }

  // Check if 6 seconds have passed or 5 peaks detected
  if (inWindow && (currentTime - windowStart >= 10000 || peakCount >= 5)) {
    if (peakCount >= 5 && (peakTimes[4] - peakTimes[0] <= 10000)) {
      Serial.println("enemy ship is made of AI Voice TS PMO SMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");  // 5 peaks within 6 seconds
    } else if (peakCount > 1) {
      // Estimate frequency based on average time between peaks
      float avgPeriodMs = (float)(peakTimes[peakCount - 1] - peakTimes[0]) / (peakCount - 1);
      float freqHz = 1000.0 / avgPeriodMs;
      Serial.print("Frequency: ");
      Serial.print(freqHz);
      Serial.println(" Hz");
    } else {
      Serial.println("Frequency: Too few peaks to estimate");
    }
    // Reset window
    inWindow = false;
    peakCount = 0;
  }

  delay(100);  // ~50 Hz sampling rate with averaging
}
