#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

#define TRIG_PIN 5
#define ECHO_PIN 18

const char* ssid = "realme 10 Pro 5G";
const char* password = "golusingh";
const char* serverUrl = "http://10.134.61.2:3000/api/sensor";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  dht.begin();
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
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
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float distance = readDistance();
    
    String jsonPayload = "{\"temperature\":" + String(temperature) + 
                        ",\"humidity\":" + String(humidity) + 
                        ",\"distance\":" + String(distance) + "}";
    
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully!");
      Serial.println("Temp: " + String(temperature) + "°C, Humidity: " + String(humidity) + "%, Distance: " + String(distance) + "cm");
    } else {
      Serial.println("Error sending data: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.reconnect();
  }
  
  delay(5000);
}