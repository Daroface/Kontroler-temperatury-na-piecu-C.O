#include <DallasTemperature.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>

//server data
const char* ssid = "KontrolerTemp";
const char* password = "kontroler";
const IPAddress host(192,168,4,1);
const uint8_t port = 801;


//temperature sensor data
#define ONE_WIRE_BUS 4 //TSENSOR connected to GPIO4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
float temperature = 0.0;


void setupWifi()
{
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("***");
    delay(500);
  }
}

void setup() 
{
  Serial.begin(9600);
  delay(100);
  setupWifi();
  Serial.println("Connected");
  sensor.begin();
}

void loop() 
{
  WiFiClient client;
  if(!client.connect(host, 801))
  {
    WiFi.begin(ssid, password);
    Serial.println("connecting...");
  }
  else
  {
    sensor.requestTemperatures();
    temperature = sensor.getTempCByIndex(0);
    temperature = temperature * 100;
    int temp = int(temperature);
    client.println(temp);
  } 
  delay(60000);

}
