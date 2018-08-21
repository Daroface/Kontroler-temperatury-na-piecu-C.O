#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>

#define ONE_WIRE_BUS 4 //TSENSOR connected to GPIO4

byte sensorPin = 13;

const char* ssid = "KontrolerTemp";
const char* password = "kontroler";
const char* host = "http://192.168.4.1";
//IPAddress host(192,168,4,1);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
WiFiClient client;

float temperature = 0;
const int TEMP_SENSOR = 13;

void setup_wifi()
{
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    delay(500);
  }
}

void setup_sensor()
{
  pinMode(TEMP_SENSOR, INPUT);
  sensor.begin();
}

void reconnect()
{
  while(!client.connected())
  {
    client.connect(host, 80);
  }
}

void setup() 
{
  Serial.begin(115200);
  delay(10);
  //************
  setup_wifi();
  //************
  setup_sensor();
}

void loop() 
{
  if(!client.connected())
  {
    reconnect();
  }
  else
  {
    sensor.setResolution(12);
    sensor.requestTemperatures();
    temperature = sensor.getTempCByIndex(0);
  //  client.publish("ha/_temp", String(temperature).c_str(), TRUE);
    client.println(temperature);
  } 
  delay(30000);

}
