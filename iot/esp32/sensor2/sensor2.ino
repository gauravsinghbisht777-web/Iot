#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <WiFiManager.h> // Make sure to install "WiFiManager" by tablatronix in your library manager!

// Sensor Pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define TRIG_PIN 5
#define ECHO_PIN 18

// Default values (will be updated by user in the setup portal)
char server_ip[40] = "10.134.61.2"; 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  dht.begin();
  
  // WiFiManager Setup
  WiFiManager wm;

  // Add a custom parameter for the Server IP
  WiFiManagerParameter custom_server_ip("server", "Server IP (e.g. 10.134.61.2)", server_ip, 40);
  wm.addParameter(&custom_server_ip);

  Serial.println("Starting Setup Portal...");
  
  // If the ESP32 can't connect to a saved Wi-Fi, it starts an Access Point
  // named "IoT-Setup-Portal". Connect your phone to it to configure settings.
  if (!wm.autoConnect("IoT-Setup-Portal")) {
    Serial.println("Failed to connect or connection timed out.");
    delay(3000);
    ESP.restart();
  }

  // Once connected, get the IP address entered by the user
  strcpy(server_ip, custom_server_ip.getValue());
  
  Serial.println("\n--- SUCCESS ---");
  Serial.println("Connected to Wi-Fi!");
  Serial.println("Server IP set to: " + String(server_ip));
  Serial.println("IP Address of ESP32: " + WiFi.localIP().toString());
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = (duration * 0.0343) / 2;
  return distance;
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Build the complete API URL using the server IP from the portal
    String serverUrl = "http://" + String(server_ip) + ":3000/api/sensor";
    
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float distance = readDistance();
    
    // Create the JSON data string
    String jsonPayload = "{\"temperature\":" + String(temperature) + 
                        ",\"humidity\":" + String(humidity) + 
                        ",\"distance\":" + String(distance) + "}";
    
    Serial.print("Sending Data to " + String(server_ip) + "... ");
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.println("OK! (" + String(httpResponseCode) + ")");
    } else {
      Serial.println("Error: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("Wi-Fi disconnected! Trying to reconnect...");
    WiFi.begin(); // Attempt to reconnect using saved credentials
  }
  
  delay(5000); // Wait 5 seconds before next reading
}