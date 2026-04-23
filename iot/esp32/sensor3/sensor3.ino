#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <WiFiManager.h> // Make sure to install "WiFiManager" by tablatronix

// Sensor Pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define TRIG_PIN 5
#define ECHO_PIN 18
#define RESET_PIN 0 // Most ESP32 boards use GPIO 0 for the "BOOT" button

// ============================================
// YOUR LIVE WEBSITE URL (hosted on Render)
// Non-developers don't need to change anything!
// ============================================
const char* serverUrl = "https://iot-9ok1.onrender.com/api/sensor";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RESET_PIN, INPUT_PULLUP); // Setup the BOOT button
  
  dht.begin();
  
  WiFiManager wm;

  // CHECK FOR RESET BUTTON PRESS AT START
  // If you hold the BOOT button while turning it on (or for 3 seconds), it clears settings
  Serial.println("Checking for manual reset...");
  delay(1000);
  if (digitalRead(RESET_PIN) == LOW) {
    Serial.println("Reset Button Pressed! Clearing Wi-Fi Settings...");
    wm.resetSettings();
    delay(1000);
  }

  // No server IP field needed anymore — it's hardcoded above!
  // The portal only asks for WiFi name and password.

  // Keep the setup portal open for 3 minutes (180 seconds)
  // so the user has enough time to connect and configure WiFi
  wm.setConfigPortalTimeout(180);

  Serial.println("Starting Setup Portal... (will stay open for 3 minutes)");
  if (!wm.autoConnect("IoT-Setup-Portal")) {
    Serial.println("No one connected. Restarting to try again...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("\nCONNECTED to WiFi!");
  Serial.println("Sending data to: " + String(serverUrl));
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return (duration * 0.0343) / 2;
}

void loop() {
  // Check if Reset button is held for 3 seconds during operation
  if (digitalRead(RESET_PIN) == LOW) {
    long startTime = millis();
    while(digitalRead(RESET_PIN) == LOW) {
      if (millis() - startTime > 3000) {
        Serial.println("FACTORY RESET: Clearing Settings & Restarting...");
        WiFiManager wm;
        wm.resetSettings();
        ESP.restart();
      }
      delay(100);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Allow HTTPS without certificate (safe for IoT)

    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float distance = readDistance();
    
    String jsonPayload = "{\"temperature\":" + String(temperature) + 
                        ",\"humidity\":" + String(humidity) + 
                        ",\"distance\":" + String(distance) + "}";
    
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.println("Data sent! Response: " + String(httpResponseCode));
    } else {
      Serial.println("Error sending data: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
  }
  delay(5000); 
}
