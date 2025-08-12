#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
const char* ssid = "Your_SSID";
const char* password = "Password";

// === Pin Definitions ===
#define ONE_WIRE_BUS 4         // DS18B20
#define FLOW_SENSOR_PIN 13     // YFS201
#define RELAY_PIN 14
#define RED_LED_PIN 33
#define GREEN_LED_PIN 32

// === Flow Sensor Variables ===
volatile int flowPulseCount = 0;
unsigned long lastFlowMillis = 0;
float flowRate = 0.0;

// === DS18B20 Temperature Sensor ===
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature = 0.0;

// === Accelerometer ===
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
float ax = 0.0, ay = 0.0, az = 9.8;

// === Control State ===
bool relayState = true;
bool manualOverride = false;

WebServer server(80);

void IRAM_ATTR flowISR() {
  flowPulseCount++;
}

// === Read Real Sensor Values ===
void updateSensorValues() {
  // --- Temperature ---
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  // --- Flow Rate ---
  unsigned long now = millis();
  unsigned long elapsed = now - lastFlowMillis;
  if (elapsed >= 1000) { // every 1 second
    detachInterrupt(FLOW_SENSOR_PIN);
    float flowLitersPerMin = (flowPulseCount / 7.5); // For YFS201: 7.5 pulses/sec = 1 L/min
    flowRate = flowLitersPerMin;
    flowPulseCount = 0;
    lastFlowMillis = now;
    attachInterrupt(FLOW_SENSOR_PIN, flowISR, RISING);
  }

  // --- Accelerometer ---
  sensors_event_t event;
  accel.getEvent(&event);
  ax = event.acceleration.x;
  ay = event.acceleration.y;
  az = event.acceleration.z;
}

void handleRoot() {
  updateSensorValues();

  // Auto-shutdown logic
  if (!manualOverride) {
    if (temperature > 30.0 || flowRate > 5.6 || az < 8.0) {
      relayState = false;
    }
  }

  // Relay and LED control
  digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
  digitalWrite(GREEN_LED_PIN, relayState ? HIGH : LOW);
  digitalWrite(RED_LED_PIN, relayState ? LOW : HIGH);

  // Velocity Calculation
  float velocity = flowRate / (1000.0 * 60.0 * 0.0038); // m/s

  // Serial log
  Serial.printf("Temp: %.2f °C | Flow: %.2f L/min | Velocity: %.4f m/s | Accel X=%.2f Y=%.2f Z=%.2f | Relay: %s\n",
                temperature, flowRate, velocity, ax, ay, az, relayState ? "ON" : "OFF");

  // HTML
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='2'>";
  html += "<title>ESP32 Sensor Dashboard</title></head><body>";
  html += "<h2>Sensor Dashboard</h2>";
  html += "<p><b>Temperature:</b> " + String(temperature) + " °C</p>";
  html += "<p><b>Flow Rate:</b> " + String(flowRate) + " L/min</p>";
  html += "<p><b>Velocity:</b> " + String(velocity, 4) + " m/s</p>";
  html += "<p><b>Accelerometer:</b> X=" + String(ax) + " Y=" + String(ay) + " Z=" + String(az) + "</p>";
  html += "<p><b>Relay:</b> " + String(relayState ? "ON" : "OFF") + "</p>";
  html += "<a href=\"/relay/on\"><button>Relay ON</button></a> ";
  html += "<a href=\"/relay/off\"><button>Relay OFF</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleRelayOn() {
  relayState = true;
  manualOverride = true;
  digitalWrite(RELAY_PIN, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRelayOff() {
  relayState = false;
  manualOverride = true;
  digitalWrite(RELAY_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(9600);

  // Pin Modes
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  digitalWrite(RELAY_PIN, HIGH); // Relay initially ON

  // Sensor Initialization
  sensors.begin(); // DS18B20
  if (!accel.begin()) {
    Serial.println("ADXL345 not detected.");
  } else {
    Serial.println("ADXL345 initialized.");
  }

  // Flow sensor interrupt
  attachInterrupt(FLOW_SENSOR_PIN, flowISR, RISING);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? "\nConnected! IP: " + WiFi.localIP().toString() : "\nFailed to connect.");

  // Server routes
  server.on("/", handleRoot);
  server.on("/relay/on", handleRelayOn);
  server.on("/relay/off", handleRelayOff);
  server.begin();
}

void loop() {
  server.handleClient();
}
