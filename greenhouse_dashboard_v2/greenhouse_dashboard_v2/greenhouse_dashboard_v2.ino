#include <Wire.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "webpage.h"

ESP8266WebServer server(80);

#define SHT20_ADDR 0x40
#define NTC_PIN A0

// Current pin assignments from your uploaded code
#define AIR_PUMP_PIN   D7
#define PLANT_LED_PIN  D3
#define WATER_PUMP_PIN D5
#define HEATER_PIN     D6
#define COLD_STEAM_PIN D8

const char* ap_ssid = "Greenhouse_AP";
const char* ap_password = "12345678";

const float SERIES_RESISTOR = 100000.0;
const float NOMINAL_RESISTANCE = 100000.0;
const float NOMINAL_TEMPERATURE = 25.0;
const float BETA = 3950.0;

unsigned long clockTimer = 0;
unsigned long sensorTimer = 0;

unsigned long airPumpTimer = 0;
unsigned long waterTimer = 0;
unsigned long ledTimer = 0;
unsigned long steamTimer = 0;

int hours = 0;
int minutes = 0;
int seconds = 0;

float airTemp = 0;
float humidity = 0;
float ntcTemp = 0;

bool airPumpState = false;
bool waterState = false;
bool ledState = false;
bool steamState = false;
bool heaterState = false;

bool airPumpAuto = true;
bool waterAuto = true;
bool ledAuto = true;
bool steamAuto = true;

// Durations are stored in seconds
unsigned long airPumpOnTime = 10;
unsigned long airPumpOffTime = 60;

unsigned long waterOnTime = 5;
unsigned long waterOffTime = 20;

unsigned long ledOnTime = 10;
unsigned long ledOffTime = 10;

unsigned long steamOnTime = 5;
unsigned long steamOffTime = 20;

float readSHT20Temperature() {
  Wire.beginTransmission(SHT20_ADDR);
  Wire.write(0xF3);
  Wire.endTransmission();
  delay(100);

  Wire.requestFrom(SHT20_ADDR, 2);
  if (Wire.available() < 2) return -999;

  uint16_t raw = Wire.read() << 8 | Wire.read();
  raw &= 0xFFFC;

  return -46.85 + 175.72 * raw / 65536.0;
}

float readSHT20Humidity() {
  Wire.beginTransmission(SHT20_ADDR);
  Wire.write(0xF5);
  Wire.endTransmission();
  delay(100);

  Wire.requestFrom(SHT20_ADDR, 2);
  if (Wire.available() < 2) return -999;

  uint16_t raw = Wire.read() << 8 | Wire.read();
  raw &= 0xFFFC;

  return -6.0 + 125.0 * raw / 65536.0;
}

float readNTC() {
  int adc = analogRead(NTC_PIN);
  if (adc <= 0 || adc >= 1023) return -999;

  float resistance = SERIES_RESISTOR * (float)adc / (1023.0 - adc);

  float temp = resistance / NOMINAL_RESISTANCE;
  temp = log(temp);
  temp /= BETA;
  temp += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
  temp = 1.0 / temp;
  temp -= 273.15;

  return temp;
}

String timeString() {
  String t = "";

  if (hours < 10) t += "0";
  t += String(hours);
  t += ":";

  if (minutes < 10) t += "0";
  t += String(minutes);
  t += ":";

  if (seconds < 10) t += "0";
  t += String(seconds);

  return t;
}

String sensorValue(float value, uint8_t decimals) {
  if (value <= -998) return "--";
  return String(value, decimals);
}

String stateText(bool state) {
  return state ? "ON" : "OFF";
}

String modeText(bool autoMode) {
  return autoMode ? "AUTO" : "MANUAL";
}

String stateClass(bool state) {
  return state ? "status-on" : "status-off";
}

String modeClass(bool autoMode) {
  return autoMode ? "mode-auto" : "mode-manual";
}

int gaugeOffset(float value, float maxValue) {
  if (value < 0) value = 0;
  if (value > maxValue) value = maxValue;

  const float circumference = 314.0;
  float fraction = value / maxValue;
  return (int)(circumference * (1.0 - fraction));
}

void redirectHome() {
  server.sendHeader("Location", "/");
  server.send(303);
}

void setAirPump(bool state) {
  airPumpState = state;
  digitalWrite(AIR_PUMP_PIN, airPumpState ? HIGH : LOW);
}

void setWater(bool state) {
  waterState = state;
  digitalWrite(WATER_PUMP_PIN, waterState ? HIGH : LOW);
}

void setLed(bool state) {
  ledState = state;
  digitalWrite(PLANT_LED_PIN, ledState ? HIGH : LOW);
}

void setSteam(bool state) {
  steamState = state;
  digitalWrite(COLD_STEAM_PIN, steamState ? HIGH : LOW);
}

void setHeater(bool state) {
  heaterState = state;
  digitalWrite(HEATER_PIN, heaterState ? HIGH : LOW);
}

void updateAutoDevice(bool &state, bool autoMode, unsigned long &timer,
                      unsigned long onTime, unsigned long offTime,
                      void (*setFunc)(bool)) {
  if (!autoMode) return;

  unsigned long now = millis();
  unsigned long interval = state ? onTime * 1000UL : offTime * 1000UL;

  if (now - timer >= interval) {
    timer = now;
    setFunc(!state);
  }
}

void handleSet() {
  if (server.hasArg("airOn")) {
    airPumpOnTime = server.arg("airOn").toInt();
    if (airPumpOnTime < 1) airPumpOnTime = 1;
  }

  if (server.hasArg("airOff")) {
    airPumpOffTime = server.arg("airOff").toInt();
    if (airPumpOffTime < 1) airPumpOffTime = 1;
  }

  if (server.hasArg("waterOn")) {
    waterOnTime = server.arg("waterOn").toInt();
    if (waterOnTime < 1) waterOnTime = 1;
  }

  if (server.hasArg("waterOff")) {
    waterOffTime = server.arg("waterOff").toInt();
    if (waterOffTime < 1) waterOffTime = 1;
  }

  if (server.hasArg("ledOn")) {
    ledOnTime = server.arg("ledOn").toInt();
    if (ledOnTime < 1) ledOnTime = 1;
  }

  if (server.hasArg("ledOff")) {
    ledOffTime = server.arg("ledOff").toInt();
    if (ledOffTime < 1) ledOffTime = 1;
  }

  if (server.hasArg("steamOn")) {
    steamOnTime = server.arg("steamOn").toInt();
    if (steamOnTime < 1) steamOnTime = 1;
  }

  if (server.hasArg("steamOff")) {
    steamOffTime = server.arg("steamOff").toInt();
    if (steamOffTime < 1) steamOffTime = 1;
  }

  unsigned long now = millis();
  airPumpTimer = now;
  waterTimer = now;
  ledTimer = now;
  steamTimer = now;

  redirectHome();
}

void handleAirPumpToggle() {
  airPumpAuto = false;
  setAirPump(!airPumpState);
  airPumpTimer = millis();
  redirectHome();
}

void handleWaterToggle() {
  waterAuto = false;
  setWater(!waterState);
  waterTimer = millis();
  redirectHome();
}

void handleLedToggle() {
  ledAuto = false;
  setLed(!ledState);
  ledTimer = millis();
  redirectHome();
}

void handleSteamToggle() {
  steamAuto = false;
  setSteam(!steamState);
  steamTimer = millis();
  redirectHome();
}

void handleHeaterToggle() {
  setHeater(!heaterState);
  redirectHome();
}

void handleAirMode() {
  airPumpAuto = !airPumpAuto;
  airPumpTimer = millis();
  redirectHome();
}

void handleWaterMode() {
  waterAuto = !waterAuto;
  waterTimer = millis();
  redirectHome();
}

void handleLedMode() {
  ledAuto = !ledAuto;
  ledTimer = millis();
  redirectHome();
}

void handleSteamMode() {
  steamAuto = !steamAuto;
  steamTimer = millis();
  redirectHome();
}

void handleRoot() {
  String html = FPSTR(DASHBOARD_HTML);
  html.reserve(15000);

  html.replace("%AIR_TEMP%", sensorValue(airTemp, 1));
  html.replace("%HUMIDITY%", sensorValue(humidity, 0));
  html.replace("%NTC_TEMP%", sensorValue(ntcTemp, 1));
  html.replace("%UPTIME%", timeString());

  html.replace("%TEMP_OFFSET%", String(gaugeOffset(airTemp, 50.0)));
  html.replace("%HUM_OFFSET%", String(gaugeOffset(humidity, 100.0)));

  html.replace("%AIR_STATE%", stateText(airPumpState));
  html.replace("%AIR_STATE_CLASS%", stateClass(airPumpState));
  html.replace("%AIR_MODE%", modeText(airPumpAuto));
  html.replace("%AIR_MODE_CLASS%", modeClass(airPumpAuto));
  html.replace("%AIR_ACTION%", airPumpState ? "Turn OFF" : "Turn ON");
  html.replace("%AIR_ACTION_CLASS%", airPumpState ? "danger" : "success");
  html.replace("%AIR_ON%", String(airPumpOnTime));
  html.replace("%AIR_OFF%", String(airPumpOffTime));

  html.replace("%WATER_STATE%", stateText(waterState));
  html.replace("%WATER_STATE_CLASS%", stateClass(waterState));
  html.replace("%WATER_MODE%", modeText(waterAuto));
  html.replace("%WATER_MODE_CLASS%", modeClass(waterAuto));
  html.replace("%WATER_ACTION%", waterState ? "Turn OFF" : "Turn ON");
  html.replace("%WATER_ACTION_CLASS%", waterState ? "danger" : "success");
  html.replace("%WATER_ON%", String(waterOnTime));
  html.replace("%WATER_OFF%", String(waterOffTime));

  html.replace("%LED_STATE%", stateText(ledState));
  html.replace("%LED_STATE_CLASS%", stateClass(ledState));
  html.replace("%LED_MODE%", modeText(ledAuto));
  html.replace("%LED_MODE_CLASS%", modeClass(ledAuto));
  html.replace("%LED_ACTION%", ledState ? "Turn OFF" : "Turn ON");
  html.replace("%LED_ACTION_CLASS%", ledState ? "danger" : "success");
  html.replace("%LED_ON%", String(ledOnTime));
  html.replace("%LED_OFF%", String(ledOffTime));

  html.replace("%STEAM_STATE%", stateText(steamState));
  html.replace("%STEAM_STATE_CLASS%", stateClass(steamState));
  html.replace("%STEAM_MODE%", modeText(steamAuto));
  html.replace("%STEAM_MODE_CLASS%", modeClass(steamAuto));
  html.replace("%STEAM_ACTION%", steamState ? "Turn OFF" : "Turn ON");
  html.replace("%STEAM_ACTION_CLASS%", steamState ? "danger" : "success");
  html.replace("%STEAM_ON%", String(steamOnTime));
  html.replace("%STEAM_OFF%", String(steamOffTime));

  html.replace("%HEATER_STATE%", stateText(heaterState));
  html.replace("%HEATER_STATE_CLASS%", stateClass(heaterState));
  html.replace("%HEATER_ACTION%", heaterState ? "Turn OFF" : "Turn ON");
  html.replace("%HEATER_ACTION_CLASS%", heaterState ? "danger" : "success");

  server.send(200, "text/html", html);
}

void startHotspot() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
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

  startHotspot();

  server.on("/", handleRoot);
  server.on("/set", handleSet);

  server.on("/air/toggle", handleAirPumpToggle);
  server.on("/water/toggle", handleWaterToggle);
  server.on("/led/toggle", handleLedToggle);
  server.on("/steam/toggle", handleSteamToggle);
  server.on("/heater/toggle", handleHeaterToggle);

  server.on("/air/mode", handleAirMode);
  server.on("/water/mode", handleWaterMode);
  server.on("/led/mode", handleLedMode);
  server.on("/steam/mode", handleSteamMode);

  server.begin();

  unsigned long now = millis();
  airPumpTimer = now;
  waterTimer = now;
  ledTimer = now;
  steamTimer = now;
}

void loop() {
  server.handleClient();

  updateAutoDevice(airPumpState, airPumpAuto, airPumpTimer,
                   airPumpOnTime, airPumpOffTime, setAirPump);

  updateAutoDevice(waterState, waterAuto, waterTimer,
                   waterOnTime, waterOffTime, setWater);

  updateAutoDevice(ledState, ledAuto, ledTimer,
                   ledOnTime, ledOffTime, setLed);

  updateAutoDevice(steamState, steamAuto, steamTimer,
                   steamOnTime, steamOffTime, setSteam);

  unsigned long now = millis();

  if (now - clockTimer >= 1000) {
    clockTimer = now;
    seconds++;

    if (seconds >= 60) {
      seconds = 0;
      minutes++;
    }

    if (minutes >= 60) {
      minutes = 0;
      hours++;
    }

    if (hours >= 24) {
      hours = 0;
    }
  }

  if (now - sensorTimer >= 2000) {
    sensorTimer = now;

    airTemp = readSHT20Temperature();
    humidity = readSHT20Humidity();
    ntcTemp = readNTC();
  }
}
