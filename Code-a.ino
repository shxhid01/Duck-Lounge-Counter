int pirPin1 = 16;   // D0 - First sensor
int pirPin2 = 5;    // D1 - Second sensor

int motionStatus1 = 0;
int motionStatus2 = 0;

int pirState1 = LOW;
int pirState2 = LOW;

int counter = 0; // Counter to be incremented or decremented

unsigned long lastMotionTime1 = 0;
unsigned long lastMotionTime2 = 0;

unsigned long debounceDelay = 2000;  // Delay in milliseconds to filter noise
unsigned long disableTime = 0;      // Time to disable the opposite sensor
unsigned long disableDuration = 2000; // Duration to disable the other sensor (2 seconds)

String inputString = "";  // Declare the inputString variable

void setup() {
  pinMode(pirPin1, INPUT);
  pinMode(pirPin2, INPUT);
  Serial.begin(115200);
  delay(3000); // Allow PIR sensors to stabilize
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if the second sensor is disabled
  bool sensor2Disabled = (currentMillis - disableTime < disableDuration);
  
  // Check if the first sensor is disabled
  bool sensor1Disabled = (currentMillis - disableTime < disableDuration);

  // Read sensor 1 if it is not disabled
  if (!sensor2Disabled) {
    motionStatus1 = digitalRead(pirPin1);
    // Check for motion on the first sensor with debounce
    if (motionStatus1 == HIGH && pirState1 == LOW && (currentMillis - lastMotionTime1) > debounceDelay) {
      Serial.println("Motion Detected on Sensor 1");
      pirState1 = HIGH;
      counter++;  // Increment counter when sensor 1 detects motion
      Serial.print("Counter: ");
      Serial.println(counter);
      lastMotionTime1 = currentMillis; // Update last motion time

      // Disable sensor 2 for a duration
      disableTime = currentMillis;
    } else if (motionStatus1 == LOW && pirState1 == HIGH && (currentMillis - lastMotionTime1) > debounceDelay) {
      Serial.println("Motion Ended on Sensor 1");
      pirState1 = LOW;
      lastMotionTime1 = currentMillis; // Update last motion time
    }
  }

  // Read sensor 2 if it is not disabled
  if (!sensor1Disabled) {
    motionStatus2 = digitalRead(pirPin2);
    // Check for motion on the second sensor with debounce
    if (motionStatus2 == HIGH && pirState2 == LOW && (currentMillis - lastMotionTime2) > debounceDelay) {
      Serial.println("Motion Detected on Sensor 2");
      pirState2 = HIGH;
      if (counter > 0) {
        counter--;  // Decrement counter when sensor 2 detects motion, but ensure counter doesn't go below zero
      }
      Serial.print("Counter: ");
      Serial.println(counter);
      lastMotionTime2 = currentMillis; // Update last motion time

      // Disable sensor 1 for a duration
      disableTime = currentMillis;
    } else if (motionStatus2 == LOW && pirState2 == HIGH && (currentMillis - lastMotionTime2) > debounceDelay) {
      Serial.println("Motion Ended on Sensor 2");
      pirState2 = LOW;
      lastMotionTime2 = currentMillis; // Update last motion time
    }
  }
  
  // Read serial input and reset counter if "RESET" message is received
  if (Serial.available() > 0) {
    char incomingChar = Serial.read();
    inputString += incomingChar;

    // If the incoming message is "RESET", reset the counter
    if (inputString.endsWith("RESET")) {
      counter = 0;  // Reset the counter
      Serial.println("Counter has been reset.");
      inputString = ""; // Clear the input string
    }
  }
}
