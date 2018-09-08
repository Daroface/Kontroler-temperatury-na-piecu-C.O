#include <OLED.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

//server data
const char* ssid = "KontrolerTemp";
const char* password = "kontroler";
WiFiServer server(801);

//pins
static const uint8_t buzzer = 16;
static const uint8_t choose_button = 12;
static const uint8_t up_button = 13;
static const uint8_t down_button = 14;
static const uint8_t sda = 4;
static const uint8_t scl = 5;

//program
OLED display(sda, scl);
const int HIGH_LIMIT = 0;
const int LOW_LIMIT = 1;
int limitLevel = 1;
float limit_temp[] = {60.0, 30.0};
float temp_value = 0.0;
String head_of_msg = "Temperatura: ";
String msg = "";
char* msgOLED = "";
String temperature = "";


void setupPins()
{
  pinMode(buzzer, OUTPUT);
  pinMode(choose_button, INPUT);
  pinMode(up_button, INPUT);
  pinMode(down_button, INPUT);
  digitalWrite(buzzer, HIGH);
  attachInterrupt(digitalPinToInterrupt(choose_button), changeTempLimits, RISING);
  attachInterrupt(digitalPinToInterrupt(up_button), increaseLimit, RISING);
  attachInterrupt(digitalPinToInterrupt(down_button), decreaseLimit, RISING);
}

void setupOLED()
{
   display.begin();
   display.print("Witaj");
   delay(1500);
}

void setup()
{
  Serial.begin(9600);  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
  Serial.println("OLED test");
  setupOLED();
  //scan();
  setupPins();
}

void printTemperatureMessageOnOLED()
{
  msg = head_of_msg + String(temp_value) + String("\r\n");  
  //msg.toCharArray(msgOLED, msg.length() + 1);
  //display.clear();
  //display.print(msgOLED);
  Serial.print(msg);
}

void printLimitTemperatureMessageOnOLED(int level)
{
  if (level == 0)
    msg =  String("Gorna: ") + limit_temp[HIGH_LIMIT] + String("\r\n");
  else if (level == 1)
    msg = String("Dolna: ") + limit_temp[LOW_LIMIT] + String("\r\n");
  else if (level == 2)
  {
    msg =  String("Gorna: ") + limit_temp[HIGH_LIMIT] + String("\r\nDolna: ") + limit_temp[LOW_LIMIT];
    if(limitLevel == 0)
      msg = msg + String("\r\nWybrana: Gorna\r\n");
    else 
      msg = msg + String("\r\nWybrana: Dolna\r\n");
  }

  //msg.toCharArray(msgOLED, msg.length() + 1);
  //display.clear();
  //display.print(msgOLED);
  Serial.print(msg);
  delay(1500);
  checkTemperature();
  loop();
}

void checkTemperature()
{
  if ((temp_value <= limit_temp[LOW_LIMIT]) || (temp_value >= limit_temp[HIGH_LIMIT]))
    digitalWrite(buzzer, LOW);
  else
    digitalWrite(buzzer, HIGH);
}

void increaseLimit()
{
  if(limit_temp[0] > limit_temp[1])
    limit_temp[limitLevel] += 5.0;
  if(limit_temp[0] <= limit_temp[1])
    limit_temp[limitLevel] -= 5.0;
  printLimitTemperatureMessageOnOLED(limitLevel);
}

void decreaseLimit()
{
  if(limit_temp[0] > limit_temp[1])
    limit_temp[limitLevel] -= 5.0;
  if(limit_temp[0] <= limit_temp[1])
    limit_temp[limitLevel] += 5.0;
  printLimitTemperatureMessageOnOLED(limitLevel);
}

void changeTempLimits()
{
  if(limitLevel == 0)
    limitLevel = 1;
  else
    limitLevel = 0;
  printLimitTemperatureMessageOnOLED(2);
  delay(2000);
  printTemperatureMessageOnOLED();
  checkTemperature();
}

void scan()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    Serial.println(error);
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(5000);           // wait 5 seconds for next scan
}

void detachInterrupts()
{
  detachInterrupt(digitalPinToInterrupt(up_button));
  detachInterrupt(digitalPinToInterrupt(choose_button));
  detachInterrupt(digitalPinToInterrupt(down_button));
}

void attachInterrupts()
{
  attachInterrupt(digitalPinToInterrupt(choose_button), changeTempLimits, RISING);
  attachInterrupt(digitalPinToInterrupt(up_button), increaseLimit, RISING);
  attachInterrupt(digitalPinToInterrupt(down_button), decreaseLimit, RISING);
}
void loop()
{
  
  WiFiClient client = server.available();
  if (client)
  {
    detachInterrupts();
    if(client.connected())
    {
        temperature = client.readStringUntil('\n');        
        int tmp = temperature.toInt();
        temp_value = tmp / 100.0;
        printTemperatureMessageOnOLED();
        checkTemperature();
    }
    client.stop();
    attachInterrupts();
  }
  temperature = "";
  msg = "";
}
