#include <Wire.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "webpage.h"

ESP8266WebServer server(80);

#define SHT20_ADDR 0x40
#define NTC_PIN A0

// Kept exactly as in your working V2 hardware setup
#define AIR_PUMP_PIN   D7
#define PLANT_LED_PIN  D3
#define WATER_PUMP_PIN D5
#define HEATER_PIN     D6
#define COLD_STEAM_PIN D8

const char* AP_SSID = "Greenhouse_AP";
const char* AP_PASSWORD = "12345678";

const float SERIES_RESISTOR = 100000.0f;
const float NOMINAL_RESISTANCE = 100000.0f;
const float NOMINAL_TEMPERATURE = 25.0f;
const float BETA = 3950.0f;

const unsigned long SENSOR_INTERVAL_MS = 2000UL;
const unsigned long MAX_TIMER_SECONDS = 604800UL; // 7 days

unsigned long sensorTimer = 0;

unsigned long airPumpTimer = 0;
unsigned long waterTimer = 0;
unsigned long ledTimer = 0;
unsigned long steamTimer = 0;

float airTemp = NAN;
float humidity = NAN;
float ntcTemp = NAN;

bool airPumpState = false;
bool waterState = false;
bool ledState = false;
bool steamState = false;
bool heaterState = false;

bool airPumpAuto = true;
bool waterAuto = true;
bool ledAuto = true;
bool steamAuto = true;

// All timing values are in seconds
unsigned long airPumpOnTime = 10;
unsigned long airPumpOffTime = 60;

unsigned long waterOnTime = 5;
unsigned long waterOffTime = 20;

unsigned long ledOnTime = 10;
unsigned long ledOffTime = 10;

unsigned long steamOnTime = 5;
unsigned long steamOffTime = 20;


// --------------------------------------------------
// Sensor functions
// --------------------------------------------------

float readSHT20Temperature() {
  Wire.beginTransmission(SHT20_ADDR);
  Wire.write(0xF3);

  if (Wire.endTransmission() != 0) {
    return NAN;
  }

  delay(90);

  if (Wire.requestFrom(SHT20_ADDR, (uint8_t)2) != 2) {
    return NAN;
  }

  uint16_t raw = ((uint16_t)Wire.read() << 8) | Wire.read();
  raw &= 0xFFFC;

  float value = -46.85f + (175.72f * raw / 65536.0f);

  if (value < -40.0f || value > 125.0f) {
    return NAN;
  }

  return value;
}

float readSHT20Humidity() {
  Wire.beginTransmission(SHT20_ADDR);
  Wire.write(0xF5);

  if (Wire.endTransmission() != 0) {
    return NAN;
  }

  delay(35);

  if (Wire.requestFrom(SHT20_ADDR, (uint8_t)2) != 2) {
    return NAN;
  }

  uint16_t raw = ((uint16_t)Wire.read() << 8) | Wire.read();
  raw &= 0xFFFC;

  float value = -6.0f + (125.0f * raw / 65536.0f);

  if (value < 0.0f) value = 0.0f;
  if (value > 100.0f) value = 100.0f;

  return value;
}

float readNTC() {
  // Average several ADC readings to reduce noise.
  unsigned long total = 0;

  for (uint8_t i = 0; i < 10; i++) {
    total += analogRead(NTC_PIN);
    delay(2);
  }

  float adc = total / 10.0f;

  if (adc <= 1.0f || adc >= 1022.0f) {
    return NAN;
  }

  /*
    Wiring:
      3.3V -> NTC -> A0 -> 100K fixed resistor -> GND

    Therefore:
      R_NTC = R_FIXED * (1023 - ADC) / ADC
  */
  float resistance =
      SERIES_RESISTOR * (1023.0f - adc) / adc;

  float steinhart = resistance / NOMINAL_RESISTANCE;
  steinhart = log(steinhart);
  steinhart /= BETA;
  steinhart += 1.0f / (NOMINAL_TEMPERATURE + 273.15f);
  steinhart = 1.0f / steinhart;
  steinhart -= 273.15f;

  if (steinhart < -40.0f || steinhart > 150.0f) {
    return NAN;
  }

  return steinhart;
}

void updateSensors() {
  float newAirTemp = readSHT20Temperature();
  float newHumidity = readSHT20Humidity();
  float newNtcTemp = readNTC();

  // Preserve the most recent valid reading if one read fails.
  if (!isnan(newAirTemp)) airTemp = newAirTemp;
  if (!isnan(newHumidity)) humidity = newHumidity;
  if (!isnan(newNtcTemp)) ntcTemp = newNtcTemp;
}


// --------------------------------------------------
// Actuator functions
// --------------------------------------------------

void setAirPump(bool state) {
  airPumpState = state;
  digitalWrite(AIR_PUMP_PIN, state ? HIGH : LOW);
}

void setWater(bool state) {
  waterState = state;
  digitalWrite(WATER_PUMP_PIN, state ? HIGH : LOW);
}

void setLed(bool state) {
  ledState = state;
  digitalWrite(PLANT_LED_PIN, state ? HIGH : LOW);
}

void setSteam(bool state) {
  steamState = state;
  digitalWrite(COLD_STEAM_PIN, state ? HIGH : LOW);
}

void setHeater(bool state) {
  heaterState = state;
  digitalWrite(HEATER_PIN, state ? HIGH : LOW);
}

void updateAutoDevice(
    bool &state,
    bool autoMode,
    unsigned long &timer,
    unsigned long onTimeSeconds,
    unsigned long offTimeSeconds,
    void (*setFunction)(bool)
) {
  if (!autoMode) return;

  unsigned long now = millis();
  unsigned long durationSeconds =
      state ? onTimeSeconds : offTimeSeconds;

  unsigned long intervalMs = durationSeconds * 1000UL;

  if (now - timer >= intervalMs) {
    timer = now;
    setFunction(!state);
  }
}


// --------------------------------------------------
// Utility functions
// --------------------------------------------------

String jsonFloat(float value, uint8_t decimals) {
  if (isnan(value)) return "null";
  return String(value, decimals);
}

unsigned long readTimerArgument(
    const String &argumentName,
    unsigned long currentValue
) {
  if (!server.hasArg(argumentName)) {
    return currentValue;
  }

  long value = server.arg(argumentName).toInt();

  if (value < 1) value = 1;
  if ((unsigned long)value > MAX_TIMER_SECONDS) {
    value = MAX_TIMER_SECONDS;
  }

  return (unsigned long)value;
}

void sendNoCacheHeaders() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
}

void sendOkJson() {
  sendNoCacheHeaders();
  server.send(200, "application/json", "{\"ok\":true}");
}


// --------------------------------------------------
// Web handlers
// --------------------------------------------------

void handleRoot() {
  sendNoCacheHeaders();

  // Serve the static page directly from flash.
  // No large HTML String is copied into RAM.
  server.send_P(200, "text/html; charset=utf-8", DASHBOARD_HTML);
}

void handleStatus() {
  String json;
  json.reserve(900);

  unsigned long uptimeSeconds = millis() / 1000UL;

  json += F("{\"temperature\":");
  json += jsonFloat(airTemp, 1);

  json += F(",\"humidity\":");
  json += jsonFloat(humidity, 0);

  json += F(",\"ntc\":");
  json += jsonFloat(ntcTemp, 1);

  json += F(",\"uptime\":");
  json += uptimeSeconds;

  json += F(",\"air\":{\"state\":");
  json += airPumpState ? F("true") : F("false");
  json += F(",\"auto\":");
  json += airPumpAuto ? F("true") : F("false");
  json += F(",\"on\":");
  json += airPumpOnTime;
  json += F(",\"off\":");
  json += airPumpOffTime;
  json += F("}");

  json += F(",\"water\":{\"state\":");
  json += waterState ? F("true") : F("false");
  json += F(",\"auto\":");
  json += waterAuto ? F("true") : F("false");
  json += F(",\"on\":");
  json += waterOnTime;
  json += F(",\"off\":");
  json += waterOffTime;
  json += F("}");

  json += F(",\"led\":{\"state\":");
  json += ledState ? F("true") : F("false");
  json += F(",\"auto\":");
  json += ledAuto ? F("true") : F("false");
  json += F(",\"on\":");
  json += ledOnTime;
  json += F(",\"off\":");
  json += ledOffTime;
  json += F("}");

  json += F(",\"steam\":{\"state\":");
  json += steamState ? F("true") : F("false");
  json += F(",\"auto\":");
  json += steamAuto ? F("true") : F("false");
  json += F(",\"on\":");
  json += steamOnTime;
  json += F(",\"off\":");
  json += steamOffTime;
  json += F("}");

  json += F(",\"heater\":{\"state\":");
  json += heaterState ? F("true") : F("false");
  json += F("}}");

  sendNoCacheHeaders();
  server.send(200, "application/json", json);
}

void handleToggle() {
  if (!server.hasArg("device")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing device\"}");
    return;
  }

  String device = server.arg("device");

  if (device == "air") {
    airPumpAuto = false;
    setAirPump(!airPumpState);
    airPumpTimer = millis();
  }
  else if (device == "water") {
    waterAuto = false;
    setWater(!waterState);
    waterTimer = millis();
  }
  else if (device == "led") {
    ledAuto = false;
    setLed(!ledState);
    ledTimer = millis();
  }
  else if (device == "steam") {
    steamAuto = false;
    setSteam(!steamState);
    steamTimer = millis();
  }
  else if (device == "heater") {
    setHeater(!heaterState);
  }
  else {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid device\"}");
    return;
  }

  sendOkJson();
}

void handleMode() {
  if (!server.hasArg("device")) {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"missing device\"}");
    return;
  }

  String device = server.arg("device");
  unsigned long now = millis();

  if (device == "air") {
    airPumpAuto = !airPumpAuto;
    airPumpTimer = now;
  }
  else if (device == "water") {
    waterAuto = !waterAuto;
    waterTimer = now;
  }
  else if (device == "led") {
    ledAuto = !ledAuto;
    ledTimer = now;
  }
  else if (device == "steam") {
    steamAuto = !steamAuto;
    steamTimer = now;
  }
  else {
    server.send(400, "application/json", "{\"ok\":false,\"error\":\"invalid device\"}");
    return;
  }

  sendOkJson();
}

void handleSetTimers() {
  airPumpOnTime = readTimerArgument("airOn", airPumpOnTime);
  airPumpOffTime = readTimerArgument("airOff", airPumpOffTime);

  waterOnTime = readTimerArgument("waterOn", waterOnTime);
  waterOffTime = readTimerArgument("waterOff", waterOffTime);

  ledOnTime = readTimerArgument("ledOn", ledOnTime);
  ledOffTime = readTimerArgument("ledOff", ledOffTime);

  steamOnTime = readTimerArgument("steamOn", steamOnTime);
  steamOffTime = readTimerArgument("steamOff", steamOffTime);

  unsigned long now = millis();
  airPumpTimer = now;
  waterTimer = now;
  ledTimer = now;
  steamTimer = now;

  sendOkJson();
}

void handleNotFound() {
  server.send(404, "application/json", "{\"ok\":false,\"error\":\"not found\"}");
}


// --------------------------------------------------
// Setup and loop
// --------------------------------------------------

void startHotspot() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
}

void setup() {
  pinMode(AIR_PUMP_PIN, OUTPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(PLANT_LED_PIN, OUTPUT);
  pinMode(COLD_STEAM_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);

  setAirPump(false);
  setWater(false);
  setLed(false);
  setSteam(false);
  setHeater(false);

  Wire.begin(D2, D1);
  Wire.setClock(100000);

  startHotspot();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/toggle", HTTP_GET, handleToggle);
  server.on("/api/mode", HTTP_GET, handleMode);
  server.on("/api/timers", HTTP_GET, handleSetTimers);
  server.onNotFound(handleNotFound);

  server.begin();

  unsigned long now = millis();
  airPumpTimer = now;
  waterTimer = now;
  ledTimer = now;
  steamTimer = now;

  // Read sensors immediately, so the first page does not show empty values.
  updateSensors();
  sensorTimer = millis();
}

void loop() {
  server.handleClient();

  updateAutoDevice(
      airPumpState,
      airPumpAuto,
      airPumpTimer,
      airPumpOnTime,
      airPumpOffTime,
      setAirPump
  );

  updateAutoDevice(
      waterState,
      waterAuto,
      waterTimer,
      waterOnTime,
      waterOffTime,
      setWater
  );

  updateAutoDevice(
      ledState,
      ledAuto,
      ledTimer,
      ledOnTime,
      ledOffTime,
      setLed
  );

  updateAutoDevice(
      steamState,
      steamAuto,
      steamTimer,
      steamOnTime,
      steamOffTime,
      setSteam
  );

  unsigned long now = millis();

  if (now - sensorTimer >= SENSOR_INTERVAL_MS) {
    sensorTimer = now;
    updateSensors();
  }

  yield();
}
