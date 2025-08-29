#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//-------------------------------- WIFI Connection --------------------------------
const char* ssid = "SAM";
const char* password = "123456780";

//---------------------------- HiveMQ Credentials ----------------------------
const char* mqtt_server = "694411e8ea0a4bd7bf8c3c7b29c0a64d.s1.eu.hivemq.cloud";  
const int mqtt_port = 8883;
const char* mqtt_username = "hivemq.webclient.1756387998221";
const char* mqtt_password = "53iDWsp;rwhJ>FX*@84E";
const char* mqtt_topic = "esp32/sensors/distance";

//----------------------------- LCD Setup ------------------------------------
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
//----------------------------- Servo Setup ----------------------------------
Servo myServo;
int servoPin = 13;
int potPin = 34;

//-------------------- Ultrasonic Buzzer and Led Setup -----------------------
const int trigPin = 26;
const int echoPin = 25;
const int buzzer = 27;
const int threshold = 50;
const int Led = 2;

//------------------------------ LDR Setup -----------------------------------
int ldrPin = 35;
int ldrValue = 0;
int ldrThreshold = 2000;

//----------------------------- MQTT Setup -----------------------------------
WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

//-------------------------- Ultrasonic Read ---------------------------------
long readUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

//----------------------------- Wifi Setup ----------------------------------
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("✅ WiFi connected successfully!");
  Serial.print("📍 IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("📥 Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("🔗 Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println(" ✅ Connected to HiveMQ!");
    } else {
      Serial.print(" ❌ Failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

//------------------------ Publish and Upload Data -------------------------------
void publishSensorData(long distance) {
  if (!client.connected()) {
    reconnect();
  }
  StaticJsonDocument<200> doc;
  doc["device_id"] = "esp32-001";
  doc["timestamp"] = millis();
  if (distance > 0 && distance < 50) { 
    doc["distance"] = distance;
  } else {
    doc["distance"] = nullptr; 
  }
  
  String jsonString;
  serializeJson(doc, jsonString);

  bool published = client.publish(mqtt_topic, jsonString.c_str());
  if (published) {
    Serial.println("📤 Published: " + jsonString);
  } else {
    Serial.println("❌ Failed to publish message");
  }
}

//-------------------------- Main Setup ------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("🚀 ESP32 Sensor System Starting...");
  
  Wire.begin(21, 22);
  
 
  lcd.init();
  lcd.backlight();
  myServo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(Led, OUTPUT);
  
 
  setup_wifi();
  espClient.setInsecure(); 
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  Serial.println("✅ ESP32 setup complete!");
}

//--------------------------- Main Loop ------------------------------------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  
  int potValue = analogRead(potPin);
  int angle = map(potValue, 0, 4095, 0, 180);
  myServo.write(angle);
  
  long distance = readUltrasonicDistance();
  Serial.print("📏 Distance: ");
  Serial.println(distance);
  
  // Buzzer & LED control (your existing code)
  if (distance > 0 && distance < threshold) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(Led, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    digitalWrite(Led, LOW);
    delay(100);
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(Led, LOW);
  }
  
  
  ldrValue = analogRead(ldrPin);
  String timeOfDay = (ldrValue < ldrThreshold) ? "Night" : "Morning";
  
  
  lcd.setCursor(0, 0);
  lcd.print("Angle:");
  lcd.print(angle);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  if (distance > 0 && distance < threshold) {
    lcd.print("Dist:");
    lcd.print(distance);
    lcd.print("cm ");
  } else {
    lcd.print("No obj ");
  }
  lcd.setCursor(12, 1);
  lcd.print(timeOfDay);
  
  
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    publishSensorData(distance);
  }
  
  delay(100);
}
