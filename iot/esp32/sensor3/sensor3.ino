#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <WiFiManager.h> // Make sure to install "WiFiManager" by tablatronix

// Sensor Pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define TRIG_PIN 5
#define ECHO_PIN 18
#define RESET_PIN 0 // Most ESP32 boards use GPIO 0 for the "BOOT" button

// Default values
char server_ip[40] = "10.134.61.2"; 

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

  WiFiManagerParameter custom_server_ip("server", "Server IP (e.g. 10.134.61.2)", server_ip, 40);
  wm.addParameter(&custom_server_ip);

  Serial.println("Starting Setup Portal...");
  if (!wm.autoConnect("IoT-Setup-Portal")) {
    Serial.println("Failed to connect, restarting...");
    delay(3000);
    ESP.restart();
  }

  strcpy(server_ip, custom_server_ip.getValue());
  Serial.println("\nCONNECTED! Server: " + String(server_ip));
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
    HTTPClient http;
    String serverUrl = "http://" + String(server_ip) + ":3000/api/sensor";
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float distance = readDistance();
    
    String jsonPayload = "{\"temperature\":" + String(temperature) + 
                        ",\"humidity\":" + String(humidity) + 
                        ",\"distance\":" + String(distance) + "}";
    
    int httpResponseCode = http.POST(jsonPayload);
    http.end();
  }
  delay(5000); 
}
