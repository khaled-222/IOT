# 🚀 ESP32 Smart Sensor System
This project demonstrates an **IoT-based ESP32 Smart Sensor System** that integrates:
- 📏 **Ultrasonic sensor** for distance measurement  
- 🔆 **LDR sensor** for light intensity detection  
- 🔊 **Buzzer & LED** for alert system  
- 🎛 **Potentiometer-controlled Servo motor**  
- 🖥 **LCD display (I2C 16x2)** for real-time feedback  
- ☁️ **MQTT (HiveMQ Cloud)** for publishing sensor data securely  

---

## 🔧 Features
- Connects ESP32 to Wi-Fi and HiveMQ Cloud via **MQTT over TLS (port 8883)**  
- Publishes sensor data (`distance`, `device_id`, `timestamp`) in **JSON format**  
- Displays sensor readings and system state on **LCD**  
- Controls **servo motor** using potentiometer input  
- Provides **buzzer + LED alert** when objects are too close  
- Detects **day/night** using LDR  

---

## 📂 Project Structure
ESP32_Smart_Sensor/
│── ESP32_Smart_Sensor.ino # Main source code
│── README.md # Documentation

yaml
Copy code

---

## 📡 MQTT Setup
- **Broker:** HiveMQ Cloud  
- **Port:** `8883` (secure)  
- **Topic:** `esp32/sensors/distance`  

### Example JSON Payload:
```json
{
  "device_id": "esp32-001",
  "timestamp": 123456,
  "distance": 25
}
🛠 Hardware Components
ESP32 Development Board

Ultrasonic Sensor (HC-SR04)

Potentiometer

Servo Motor

LDR Sensor

Buzzer + LED

LCD 16x2 (I2C)

⚙️ Wiring
Component	ESP32 Pin
Ultrasonic Trig	26
Ultrasonic Echo	25
Servo Signal	13
Potentiometer	34 (ADC)
Buzzer	27
LED	2
LDR	35 (ADC)
I2C SDA	21
I2C SCL	22

📲 Setup Instructions
Install Arduino IDE and add ESP32 board support.

Install the following libraries:

ESP32Servo

LiquidCrystal_I2C

WiFi

PubSubClient

ArduinoJson

Update WiFi SSID & password in the code:

cpp
Copy code
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
Update HiveMQ credentials:

cpp
Copy code
const char* mqtt_server   = "your-broker-url";
const int   mqtt_port     = 8883;
const char* mqtt_username = "your-username";
const char* mqtt_password = "your-password";
Upload code to ESP32.

Open Serial Monitor to view system logs.

📊 Example Serial Output
arduino
Copy code
🚀 ESP32 Sensor System Starting...
✅ WiFi connected successfully!
📍 IP address: 192.168.1.10
🔗 Attempting MQTT connection... ✅ Connected to HiveMQ!
📏 Distance: 23 cm
📤 Published: {"device_id":"esp32-001","timestamp":5123,"distance":23}
📝 Notes
Distance alerts are triggered when object < 50 cm.

LDR threshold (2000) distinguishes Morning/Night.

Data is published every 5 second
