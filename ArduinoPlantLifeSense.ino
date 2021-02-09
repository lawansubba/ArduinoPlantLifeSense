/*
  MKR ENV Shield - Read Sensors

  This example reads the sensors on-board the MKR ENV shield
  and prints them to the Serial Monitor once a second.

  The circuit:
  - Arduino MKR board
  - Arduino MKR ENV Shield attached

  This example code is in the public domain.
*/

#include <Arduino_MKRENV.h>
#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include <ArduinoJson.h>


#include "arduino_secrets.h"

const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }

  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1);
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();
  
  if (millis() - lastMillis > 900000)  {
    // publish a message roughly every 5 seconds.
    lastMillis = millis();
    
    publishMessage();
  }
}

void publishMessage() {
  StaticJsonDocument<192> doc;

  doc["time"] = getTime();
 
  float temperature = ENV.readTemperature();
  doc["temp"] = temperature;
  
  float humidity    = ENV.readHumidity();
  doc["humd"] = humidity;
  
  float pressure    = ENV.readPressure();
  doc["press"] = pressure;
  
  float illuminance = ENV.readIlluminance();
  doc["ill"] = illuminance;
  
  float uva  = ENV.readUVA();
  doc["uva"] = uva;
  
  float uvb = ENV.readUVB();
  doc["uvb"] = uvb;
  
  float uvIndex     = ENV.readUVIndex();
  doc["uvi"] = uvIndex; 

  int soilMoistureValue = 0;
  soilMoistureValue = analogRead(A1);
  doc["mois"] = soilMoistureValue; 
    
   //serializeJson(doc, Serial);
  // Serial.println("");
  
  // send MQTT message
   mqttClient.beginMessage("arduino_outgoing_channel");
   serializeJson(doc, mqttClient);
   mqttClient.endMessage();
}

unsigned long getTime() {
  // get the current time from the WiFi module
  return WiFi.getTime();
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid,  pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();
}
