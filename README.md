# IoT-Hydroponic-Greenhouse
# 🌱 Smart Hydroponic Greenhouse Controller

A smart IoT-based hydroponic greenhouse controller built using an **ESP8266 NodeMCU**. The system provides real-time environmental monitoring and automatic/manual control of greenhouse devices through a responsive web dashboard hosted directly on the ESP8266.

---

## ✨ Features

### 🌡️ Environmental Monitoring

- Air Temperature (SHT20)
- Air Humidity (SHT20)
- Root Temperature (NTC Thermistor)
- System Uptime

---

### ⚙️ Device Control

Control every actuator individually:

- 💧 Water Pump
- 🫧 Air Pump
- 💡 Plant LED
- 🌫️ Cold Steam
- 🔥 Heater

Each device supports:

- Manual ON/OFF
- AUTO mode
- MANUAL mode
- Independent ON timer
- Independent OFF timer
- Live status monitoring

---

### 🌐 Embedded Web Dashboard

The ESP8266 hosts its own responsive web interface.

Features include:

- Modern mobile-friendly UI
- Live sensor values
- Circular gauges
- Live trend chart
- Device status cards
- Timer configuration
- AUTO/MANUAL switching
- No page refresh (AJAX updates)
- JSON REST API

The dashboard is stored directly inside the ESP8266 flash memory.

---

## 📷 Dashboard

(Add screenshots here)

Example:

```
images/dashboard-home.png
images/dashboard-devices.png
images/dashboard-mobile.png
```

---

## 🛠 Hardware

| Component | Model |
|------------|-------|
| Microcontroller | ESP8266 NodeMCU 1.0 |
| Temperature/Humidity Sensor | SHT20 |
| Root Temperature Sensor | NTC 100K |
| Water Pump | Relay/MOSFET |
| Air Pump | Relay/MOSFET |
| Plant LED | Relay/MOSFET |
| Cold Steam | Relay/MOSFET |
| Heater | Relay/MOSFET |

---

## 🔌 Pin Configuration

| Device | ESP8266 Pin |
|---------|-------------|
| Air Pump | D0 |
| Plant LED | D4 |
| Water Pump | D5 |
| Heater | D6 |
| Cold Steam | D8 |
| SHT20 SDA | D2 |
| SHT20 SCL | D1 |
| NTC | A0 |

---

## 📡 WiFi Access Point

The controller creates its own WiFi hotspot.

```
SSID: Greenhouse_AP
Password: 12345678
```

After connecting:

```
http://192.168.4.1
```

---

## 📊 Dashboard Functions

✔ Live Temperature

✔ Live Humidity

✔ Live Root Temperature

✔ Device Status

✔ Manual Control

✔ Automatic Control

✔ Adjustable Timers

✔ Live Trend Chart

✔ Mobile Responsive Layout

---

## 🤖 Automatic Control

Each actuator has completely independent timing.

Example:

| Device | ON | OFF |
|---------|----|-----|
| Air Pump | 10 sec | 120 sec |
| Water Pump | 10 sec | 120 sec |
| Plant LED | 8 hr | 16 hr |
| Cold Steam | 30 sec | 10 min |

All values can be changed directly from the dashboard.

---

## 🧠 Software Architecture

```
             Sensors
        ┌──────────────┐
        │   SHT20      │
        │   NTC100K    │
        └──────┬───────┘
               │
               ▼
      ESP8266 Firmware
               │
        Sensor Processing
               │
     Automatic Control Logic
               │
        Embedded Web Server
               │
          JSON REST API
               │
        Responsive Dashboard
               │
          User Controls
```

---

## 📁 Project Structure

```
Smart-Greenhouse/

│
├── Smart_Greenhouse.ino
├── webpage.h
├── README.md
└── images/
```

---

## ⚡ Technologies

- Arduino C++
- ESP8266WiFi
- ESP8266WebServer
- HTML5
- CSS3
- JavaScript
- AJAX
- JSON
- I2C (Wire)

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/yourusername/Smart-Greenhouse.git
```

---

### 2. Open the project

Open

```
Smart_Greenhouse.ino
```

using the Arduino IDE.

---

### 3. Install Libraries

Required libraries:

- ESP8266WiFi
- ESP8266WebServer
- Wire

---

### 4. Select Board

```
NodeMCU 1.0 (ESP-12E Module)
```

---

### 5. Upload

Compile and upload the firmware.

After booting, connect to:

```
Greenhouse_AP
```

Then open

```
http://192.168.4.1
```

---

## 📈 Planned Features

- Water Level Sensor
- Soil Moisture Sensor
- pH Sensor
- EC Sensor
- DS3231 RTC
- EEPROM Settings
- Historical Charts
- Authentication
- MQTT Support
- Home Assistant Integration
- Blynk Integration
- Alarm System
- Smart Greenhouse Automation
- Data Logging

---

## 🤝 Contributing

Contributions, ideas, and feature requests are welcome.

Feel free to open an issue or submit a pull request.

---

## 📄 License

This project is released under the MIT License.

---

## 👨‍💻 Author

**Babak Amolirad**
**Amirreza Qadami**
**Yasin Charkazi**

Smart Hydroponic Greenhouse Controller

Built as an Internet of Things (IoT) university project using the ESP8266 platform.
