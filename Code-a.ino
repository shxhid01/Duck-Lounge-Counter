#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> // For creating JSON payloads

// PIR Sensor Pins
int pirPin1 = 16; // D0 - First sensor
int pirPin2 = 5;  // D1 - Second sensor

// Replace with your network credentials
const char *ssid = "";     // Your WiFi SSID (network name)
const char *password = ""; // Your WiFi password

// Discord Webhook URL
const char *discordWebhookURL = "";

// Motion detection variables
int motionStatus1 = 0;
int motionStatus2 = 0;
int pirState1 = LOW;
int pirState2 = LOW;
int counter = 0; // Counter to be incremented or decremented

unsigned long lastMotionTime1 = 0;
unsigned long lastMotionTime2 = 0;
const unsigned long debounceDelay = 2000; // 2 seconds
unsigned long disableTime1 = 0;
unsigned long disableTime2 = 0;
const unsigned long disableDuration = 2000; // 2 seconds

String inputString = ""; // For serial input

// Function to send data to Discord
void sendToDiscord(String message)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClientSecure client; // Use WiFiClientSecure instead of WiFiClient
    client.setInsecure();    // Skip certificate verification
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

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.print("Discord response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
    }
    else
    {
      Serial.print("Error sending to Discord. Error: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  }
  else
  {
    Serial.println("WiFi not connected");
  }
}

void handleSensor(int pirPin, int &motionStatus, int &pirState, unsigned long &lastMotionTime, bool sensorDisabled, int counterDelta, const String &sensorName)
{
  unsigned long currentMillis = millis();

  if (!sensorDisabled)
  {
    motionStatus = digitalRead(pirPin);

    if (motionStatus == HIGH && pirState == LOW && (currentMillis - lastMotionTime) > debounceDelay)
    {
      Serial.println("Motion Detected on " + sensorName);
      pirState = HIGH;
      counter += counterDelta;
      Serial.print("Counter: ");
      Serial.println(counter);
      lastMotionTime = currentMillis;

      // Send message to Discord
      sendToDiscord("🚶 Motion Detected on " + sensorName + ". Counter: " + String(counter));

      // Disable the other sensor
      if (pirPin == pirPin1)
      {
        disableTime2 = currentMillis;
      }
      else
      {
        disableTime1 = currentMillis;
      }
    }
    else if (motionStatus == LOW && pirState == HIGH && (currentMillis - lastMotionTime) > debounceDelay)
    {
      Serial.println("Motion Ended on " + sensorName);
      pirState = LOW;
      lastMotionTime = currentMillis;

      // Send message to Discord
      sendToDiscord("✅ Motion Ended on " + sensorName + ". Counter: " + String(counter));
    }
  }
}

void setup()
{
  // Connect to Wi-Fi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize PIR pins
  pinMode(pirPin1, INPUT);
  pinMode(pirPin2, INPUT);
  delay(3000); // Allow PIR sensors to stabilize
}

void loop()
{
  unsigned long currentMillis = millis();

  // Check if sensors are disabled
  bool sensor1Disabled = (currentMillis - disableTime1 < disableDuration);
  bool sensor2Disabled = (currentMillis - disableTime2 < disableDuration);

  // Handle sensor 1
  handleSensor(pirPin1, motionStatus1, pirState1, lastMotionTime1, sensor1Disabled, 1, "Sensor 1");

  // Handle sensor 2
  handleSensor(pirPin2, motionStatus2, pirState2, lastMotionTime2, sensor2Disabled, -1, "Sensor 2");

  // Handle serial input for reset
  if (Serial.available() > 0)
  {
    inputString = Serial.readStringUntil('\n');
    inputString.trim();

    if (inputString.equalsIgnoreCase("reset"))
    {
      counter = 0;
      Serial.println("Counter has been reset.");
      sendToDiscord("🔄 Counter has been reset.");
    }
    inputString = "";
  }
}
