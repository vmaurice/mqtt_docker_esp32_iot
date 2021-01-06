/*
 * Author : Vincent Maurice
 * 
 * Connect ESP32 to a wifi terminal and communication with MQTT server
 * two I2C devices, one for temperature (MCP9808) and one for luminosity (TSL2561)
 * he publishes in sensors/kerno/bedroom/temperature and sensors/kerno/bedroom/luminosity
 * 
 */


#include <WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <Digital_Light_TSL2561.h>

#include <HCSR04.h>

int triggerPin = 32;
int echoPin = 35;
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);

byte upperByte;
byte lowerByte;

float temperature;
float luminosite;

// Please input the SSID and password of WiFi
const char* ssid     = "SSID";
const char* password = "PASSWORD";

const char* mqtt_server = "IP_BROKER";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(115200);
  Serial.println("Starting connecting WiFi.");
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Wire.begin();
  TSL2561.init();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32Client")) {
 
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  client.subscribe("sensors/kerno/bedroom/call/#");
}

float getTemperature() {
  float temperature = 0.0;
  
  Wire.beginTransmission(0x1A);
  
  Wire.write(0X05);

  Wire.endTransmission(false); 
  

  Wire.requestFrom(0x1A, sizeof(byte)*2);

  if(Wire.available())   
  {
    upperByte = Wire.read();
    lowerByte = Wire.read();
  }

  upperByte = upperByte & 0x1F;

  if ((upperByte & 0x10) == 0x10) {
    Serial.println("Negative value");
    upperByte = upperByte & 0x0F;
    temperature = 256 - (upperByte * 16.0 + lowerByte / 16.0);
    temperature *= -1;
  } 
  else {
    temperature = (upperByte * 16.0 + lowerByte / 16.0);
  }

  return temperature;
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message

  char buff[256];
  String txt = "";
  
  if (String(topic) == "sensors/kerno/bedroom/call/door") {
    int distance = distanceSensor.measureDistanceCm();
    if(distance > 20) {
      Serial.println("Door closed (" + String(distance) + ")");
    }
    else{
      Serial.println("Door opened (" + String(distance) + ")");
    }
    txt = "{\"value\": " + String(distance) + "}";
    snprintf(buff, 256, txt.c_str());
  
    if (client.publish("sensors/kerno/bedroom/answer/door", buff))
      Serial.println("Sending temperature succeed");
    else
      Serial.println("Failure...");
  } 
  else if (String(topic) == "sensors/kerno/bedroom/call/temp") {
    txt = "{\"value\":" + (String)temperature + ", \"unit\":\"Celsus\" }";
    snprintf(buff, 256, txt.c_str());
  
    if (client.publish("sensors/kerno/bedroom/answer/temp", buff))
      Serial.println("Sending temperature succeed");
    else
      Serial.println("Failure...");
  }
  else if (String(topic) == "sensors/kerno/bedroom/call/lum") {
    txt = "{\"value\":" + (String)luminosite + ", \"unit\":\"Lux\"}";
    snprintf(buff, 256, txt.c_str());
  
    if (client.publish("sensors/kerno/bedroom/answer/lum", buff))
      Serial.println("Sending temperature succeed");
    else
      Serial.println("Failure...");
  }
  else
  {
    Serial.println("Error topic received");
  }
}


void loop() {

  client.loop();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WIFI not coonnected");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(5000);
        Serial.print(".");
    }
  }

  while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
      if (client.connect("ESP32Client")) {
        Serial.println("connected");
      } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(5000);
      }
    }
  
  
  temperature = getTemperature();  
  luminosite = TSL2561.readVisibleLux();

  Serial.print("La temperature est de ");
  Serial.println(temperature);
  
  Serial.print("La luminosité est de ");
  Serial.println(luminosite);
  
  char buff[256];

  // replace the following line with your data sent to Azure IoTHub
  String txt = "{\"value\":" + (String)temperature + ", \"unit\":\"Celsus\" }";
  snprintf(buff, 256, txt.c_str());
  
  if (client.publish("sensors/kerno/bedroom/temperature", buff))
    Serial.println("Sending temperature succeed");
  else
    Serial.println("Failure...");

  
  txt = "{\"value\":" + (String)luminosite + ", \"unit\":\"Lux\"}";
  snprintf(buff, 256, txt.c_str());
  
  if (client.publish("sensors/kerno/bedroom/luminosity", buff))
    Serial.println("Sending luminosité succeed");
    else
      Serial.println("Failure...");
  
  delay(5000);
}