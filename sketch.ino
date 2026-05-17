// Define pins
const int ldrPin = 34;    // LDR connected to analog pin 34

// PIR sensor pins
const int pirPins[4] = {23, 21, 18, 4};

// LED pins - must be different from PIR pins
const int brightLedPins[4] = {22, 19, 5, 2};  // Changed from pin 19 to avoid conflict
const int dimLedPins[4] = {32, 33, 25, 26};     // Different set of pins

// Variables 
int pirStates[4] = {0, 0, 0, 0};
bool nightMode = false;
const int ldrThreshold = 2000;
unsigned long lastMotionTime = 0;
const unsigned long dimDelay = 5000;

void setup() {
  Serial.begin(115200);
  
  // Initialize PIR sensors as inputs
  for (int i = 0; i < 4; i++) {
    pinMode(pirPins[i], INPUT);
  }
  
  // Initialize all LEDs as outputs
  for (int i = 0; i < 4; i++) {
    pinMode(brightLedPins[i], OUTPUT);
    pinMode(dimLedPins[i], OUTPUT);
    digitalWrite(brightLedPins[i], LOW);
    digitalWrite(dimLedPins[i], LOW);
  }
}

void loop() {
  // Read LDR value
  int ldrValue = analogRead(ldrPin);
  nightMode = (ldrValue > ldrThreshold);
  
  Serial.print("LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | Night: ");
  Serial.println(nightMode ? "YES" : "NO");

  if (nightMode) {
    // Read all PIR sensors
    bool motionDetected = false;
    for (int i = 0; i < 4; i++) {
      pirStates[i] = digitalRead(pirPins[i]);
      if (pirStates[i] == HIGH) {
        motionDetected = true;
        lastMotionTime = millis();
      }
    }
    
    // Control lights
    if (motionDetected) {
      for (int i = 0; i < 4; i++) {
        if (pirStates[i] == HIGH) {
          // Activate bright LEDs for current and next 2 poles
          digitalWrite(brightLedPins[(i-1)%4], HIGH);
          digitalWrite(brightLedPins[i], HIGH);
          digitalWrite(brightLedPins[(i+1)%4], HIGH);
          
          // Deactivate dim LEDs for these poles
          digitalWrite(dimLedPins[(i-1)%4], LOW);
          digitalWrite(dimLedPins[i], LOW);
          digitalWrite(dimLedPins[(i+1)%4], LOW);
          
          // Handle the previous pole
          digitalWrite(brightLedPins[(i+3)%4], LOW);
          digitalWrite(dimLedPins[(i+3)%4], HIGH);
        }
      }
    }
    else {
      // No motion - check if we should dim
      if (millis() - lastMotionTime > dimDelay) {
        for (int i = 0; i < 4; i++) {
          digitalWrite(brightLedPins[i], LOW);
          digitalWrite(dimLedPins[i], HIGH);
        }
      }
    }
  }
  else {
    // Daytime - turn off all
    for (int i = 0; i < 4; i++) {
      digitalWrite(brightLedPins[i], LOW);
      digitalWrite(dimLedPins[i], LOW);
    }
    lastMotionTime = millis();
  }
  
  delay(100);
}