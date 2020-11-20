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

byte upperByte;
byte lowerByte;

float temperature;
float luminosite;

// Please input the SSID and password of WiFi
const char* ssid     = "SSID";
const char* password = "PASSWORD";

// Please input IP device of mqtt_server
const char* mqtt_server = "IP_OF_SERVET";

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
}

// get temperature of MCP9808 device
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
    Serial.println("Negatve value");
    upperByte = upperByte & 0x0F;
    temperature = 256 - (upperByte * 16.0 + lowerByte / 16.0);
    temperature *= -1;
  } 
  else {
    temperature = (upperByte * 16.0 + lowerByte / 16.0);
  }

  return temperature;
}


void loop() {
  
  
  temperature = getTemperature();  
  luminosite = TSL2561.readVisibleLux();

  Serial.print("La temperature est de ");
  Serial.println(temperature);
  
  Serial.print("La luminosité est de ");
  Serial.println(luminosite);
  
  char buff[256];

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
