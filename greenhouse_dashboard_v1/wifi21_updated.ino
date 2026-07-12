#include <Wire.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

#define SHT20_ADDR 0x40
#define NTC_PIN A0

// Actuator pins
#define AIR_PUMP_PIN   D7  // GPIO16
#define PLANT_LED_PIN  D3  // GPIO2
#define WATER_PUMP_PIN D5  // GPIO14
#define HEATER_PIN     D6  // GPIO12
#define COLD_STEAM_PIN D8  // GPIO15

const char* ap_ssid = "Greenhouse_AP";
const char* ap_password = "12345678";

const float SERIES_RESISTOR = 100000.0;
const float NOMINAL_RESISTANCE = 100000.0;
const float NOMINAL_TEMPERATURE = 25.0;
const float BETA = 3950.0;

unsigned long clockTimer = 0;
unsigned long sensorTimer = 0;

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

bool waterAuto = true;
bool ledAuto = true;
bool steamAuto = true;

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

  waterTimer = millis();
  ledTimer = millis();
  steamTimer = millis();

  redirectHome();
}

void handleAirPumpToggle() {
  setAirPump(!airPumpState);
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

void handleRoot() {
  String html = "";
  html.reserve(6000);

  html += "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>Hydroponic Greenhouse</title>";
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;background:#eef7ee;padding:20px;}";
  html += ".card{background:white;margin:15px auto;padding:18px;border-radius:12px;max-width:420px;box-shadow:0 0 8px #999;}";
  html += "h1{color:#1b5e20}.value{font-size:24px;font-weight:bold;color:#2e7d32;}";
  html += ".on{color:#1b8a2f}.off{color:#c62828}";
  html += "input{width:70px;padding:6px;margin:4px;}";
  html += "button{padding:9px 14px;margin:4px;border:0;border-radius:8px;cursor:pointer;}";
  html += ".onButton{background:#2e7d32;color:white}.offButton{background:#c62828;color:white}";
  html += "</style></head><body>";

  html += "<h1>Hydroponic Greenhouse</h1>";

  html += "<div class='card'><h2>Sensors</h2>";
  html += "<p>Air Temp: <b>" + String(airTemp, 1) + " &deg;C</b></p>";
  html += "<p>Humidity: <b>" + String(humidity, 0) + " %</b></p>";
  html += "<p>NTC Temp: <b>" + String(ntcTemp, 1) + " &deg;C</b></p>";
  html += "<p>Uptime: <b>" + timeString() + "</b></p>";
  html += "</div>";

  html += "<div class='card'><h2>Device Control</h2>";

  html += "<p>Air Pump: <b class='" + String(airPumpState ? "on" : "off") + "'>";
  html += airPumpState ? "ON" : "OFF";
  html += "</b></p><a href='/air/toggle'><button class='";
  html += airPumpState ? "offButton'>Turn Air Pump OFF" : "onButton'>Turn Air Pump ON";
  html += "</button></a>";

  html += "<p>Water Pump: <b class='" + String(waterState ? "on" : "off") + "'>";
  html += waterState ? "ON" : "OFF";
  html += "</b></p><a href='/water/toggle'><button class='";
  html += waterState ? "offButton'>Turn Water Pump OFF" : "onButton'>Turn Water Pump ON";
  html += "</button></a>";

  html += "<p>Plant LED: <b class='" + String(ledState ? "on" : "off") + "'>";
  html += ledState ? "ON" : "OFF";
  html += "</b></p><a href='/led/toggle'><button class='";
  html += ledState ? "offButton'>Turn Plant LED OFF" : "onButton'>Turn Plant LED ON";
  html += "</button></a>";

  html += "<p>Cold Steam: <b class='" + String(steamState ? "on" : "off") + "'>";
  html += steamState ? "ON" : "OFF";
  html += "</b></p><a href='/steam/toggle'><button class='";
  html += steamState ? "offButton'>Turn Cold Steam OFF" : "onButton'>Turn Cold Steam ON";
  html += "</button></a>";

  html += "<p>Heater: <b class='" + String(heaterState ? "on" : "off") + "'>";
  html += heaterState ? "ON" : "OFF";
  html += "</b></p><a href='/heater/toggle'><button class='";
  html += heaterState ? "offButton'>Turn Heater OFF" : "onButton'>Turn Heater ON";
  html += "</button></a>";

  html += "</div>";

  html += "<div class='card'><h2>Timing Control</h2>";
  html += "<form action='/set' method='GET'>";

  html += "<h3>Water Pump</h3>";
  html += "ON <input name='waterOn' type='number' min='1' value='" + String(waterOnTime) + "'> sec ";
  html += "OFF <input name='waterOff' type='number' min='1' value='" + String(waterOffTime) + "'> sec";

  html += "<h3>Plant LED</h3>";
  html += "ON <input name='ledOn' type='number' min='1' value='" + String(ledOnTime) + "'> sec ";
  html += "OFF <input name='ledOff' type='number' min='1' value='" + String(ledOffTime) + "'> sec";

  html += "<h3>Cold Steam</h3>";
  html += "ON <input name='steamOn' type='number' min='1' value='" + String(steamOnTime) + "'> sec ";
  html += "OFF <input name='steamOff' type='number' min='1' value='" + String(steamOffTime) + "'> sec";

  html += "<br><br><button class='onButton' type='submit'>Save Times</button>";
  html += "</form></div>";

  html += "</body></html>";

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

  server.begin();
}

void loop() {
  server.handleClient();

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
