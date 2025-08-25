#include <ArduinoJson.h> // For creating JSON payloads
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Sensor and timing configuration
constexpr unsigned long DEBOUNCE_DELAY = 2000;   // 2 seconds
constexpr unsigned long DISABLE_DURATION = 2000; // 2 seconds

struct Sensor {
  uint8_t pin;
  int motionStatus;
  int pirState;
  unsigned long lastMotionTime;
  unsigned long disableTime;
  String name;
  int counterDelta;

  Sensor(uint8_t p, int delta, const String &n)
      : pin(p), motionStatus(0), pirState(LOW), lastMotionTime(0),
        disableTime(0), name(n), counterDelta(delta) {}
};

Sensor sensor1(16, 1, "Sensor 1"); // D0 - First sensor
Sensor sensor2(5, -1, "Sensor 2"); // D1 - Second sensor

// Replace with your network credentials
const char *ssid = "";     // Your WiFi SSID (network name)
const char *password = ""; // Your WiFi password

// Discord Webhook URL
const char *discordWebhookURL = "";

int counter = 0;         // Counter to be incremented or decremented
String inputString = ""; // For serial input

// Function to send data to Discord
void sendToDiscord(const String &message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure(); // Skip certificate verification
  HTTPClient http;

  // Create JSON payload
  StaticJsonDocument<200> jsonPayload;
  jsonPayload["content"] = message;
  jsonPayload["username"] = "Motion Sensor Bot";

  String payload;
  serializeJson(jsonPayload, payload);

  // Send POST request to Discord webhook
  http.begin(client, discordWebhookURL); // Use the secure client
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP8266");

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    Serial.print("Discord response code: ");
    Serial.println(httpResponseCode);
    Serial.print("Response: ");
    Serial.println(http.getString());
  } else {
    Serial.print("Error sending to Discord. Error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}

void handleSensor(Sensor &sensor, Sensor &otherSensor) {
  unsigned long currentMillis = millis();

  if (currentMillis - sensor.disableTime < DISABLE_DURATION) {
    return; // Sensor is temporarily disabled
  }

  sensor.motionStatus = digitalRead(sensor.pin);

  if (sensor.motionStatus == HIGH && sensor.pirState == LOW &&
      (currentMillis - sensor.lastMotionTime) > DEBOUNCE_DELAY) {
    Serial.println("Motion Detected on " + sensor.name);
    sensor.pirState = HIGH;
    counter += sensor.counterDelta;
    Serial.print("Counter: ");
    Serial.println(counter);
    sensor.lastMotionTime = currentMillis;

    // Send message to Discord
    sendToDiscord("🚶 Motion Detected on " + sensor.name +
                  ". Counter: " + String(counter));

    // Disable the other sensor
    otherSensor.disableTime = currentMillis;
  } else if (sensor.motionStatus == LOW && sensor.pirState == HIGH &&
             (currentMillis - sensor.lastMotionTime) > DEBOUNCE_DELAY) {
    Serial.println("Motion Ended on " + sensor.name);
    sensor.pirState = LOW;
    sensor.lastMotionTime = currentMillis;

    // Send message to Discord
    sendToDiscord("✅ Motion Ended on " + sensor.name +
                  ". Counter: " + String(counter));
  }
}

void setup() {
  // Connect to Wi-Fi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize PIR pins
  pinMode(sensor1.pin, INPUT);
  pinMode(sensor2.pin, INPUT);
  delay(3000); // Allow PIR sensors to stabilize
}

void loop() {
  // Handle sensors
  handleSensor(sensor1, sensor2);
  handleSensor(sensor2, sensor1);

  // Handle serial input for reset
  if (Serial.available() > 0) {
    inputString = Serial.readStringUntil('\n');
    inputString.trim();

    if (inputString.equalsIgnoreCase("reset")) {
      counter = 0;
      Serial.println("Counter has been reset.");
      sendToDiscord("🔄 Counter has been reset.");
    }
    inputString = "";
  }
}
