#include <OLED.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

//server data
const char* ssid = "KontrolerTemp";
const char* password = "kontroler";
WiFiServer server(801);

//pins
static const uint8_t buzzer = 16;
static const uint8_t chooseButton = 12;
static const uint8_t upButton = 14;
static const uint8_t downButton = 13;
static const uint8_t sda = 4;
static const uint8_t scl = 5;

//program
OLED display(sda, scl);
const int HIGHLIMIT = 0;
const int LOWLIMIT = 1;
int limitLevel = 1;
float limitTemp[] = {60.0, 30.0};
float tempValue = 0.0;
const char TEMPMSG[] = "Temp: ";
const char UPLIMITMSG[] = "Gorna: ";
const char DOWNLIMITMSG[] = "Dolna: ";
const char UPMSG[] = "Gorna";
const char DOWNMSG[] = "Dolna";
const char CHOOICEMSG[] = "Wybrana: ";
char* msgOLED;


void setupPins()
{
  pinMode(buzzer, OUTPUT);
  pinMode(chooseButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  digitalWrite(buzzer, HIGH);
  attachInterrupt(digitalPinToInterrupt(chooseButton), changeTempLimits, RISING);
  attachInterrupt(digitalPinToInterrupt(upButton), increaseLimit, RISING);
  attachInterrupt(digitalPinToInterrupt(downButton), decreaseLimit, RISING);
}

void setupOLED()
{
   display.begin();
   delay(100);
   display.print("Witaj", 2, 2);
   delay(1400);
}

void setup()
{
  Serial.begin(9600);  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();
  setupOLED();  
  setupPins();
}

void insertIntoArray(const char* text, float number)
{
  int sizeOfArray = strlen(text) + 1;
  if(number >= 100.00)
  {
    sizeOfArray += 6;
  }    
  else if(number >= 10.00)
  {
    sizeOfArray += 5;
  }
  else
  {
    sizeOfArray += 4;
  }
  String tmpArray = String(number);
  msgOLED = new char[sizeOfArray];
  int j=0;
  for(int i = 0; i < sizeOfArray; i++)
  {
    if(i < strlen(text))
    {
      msgOLED[i] = text[i];
    }
    else
    {
      msgOLED[i] = tmpArray[j];
      j++;
    }
  }  
}

void insertIntoArray(const char* text, const char* text2)
{
  int sizeOfArray = strlen(text) + strlen(text2) + 1;
  msgOLED = new char[sizeOfArray];
  int j=0;
  for(int i = 0; i < sizeOfArray; i++)
  {
    if(i < strlen(text))
    {
      msgOLED[i] = text[i];
    }
    else
    {
      msgOLED[i] = text2[j];
      j++;
    }
  } 
}

void printTemperatureMessageOnOLED()
{
  insertIntoArray(TEMPMSG, tempValue);
  display.clear();
  delay(200);
  display.print(msgOLED, 2, 2);
  delay(200);
  delete msgOLED;
}

void printLimitTemperatureMessageOnOLED(int level)
{
  display.clear();
  delay(200);
  if (level == 0)
  {
    insertIntoArray(UPLIMITMSG, limitTemp[HIGHLIMIT]); 
    display.print(msgOLED, 2, 2);
    delay(200);
    delete msgOLED;
  }
  else if (level == 1)
  {
    insertIntoArray(DOWNLIMITMSG, limitTemp[LOWLIMIT]);
    display.print(msgOLED, 2, 2);
    delay(200);
    delete msgOLED;
  }    
  else if (level == 2)
  {
    insertIntoArray(UPLIMITMSG, limitTemp[HIGHLIMIT]);   
    display.print(msgOLED, 2, 2);
    delay(200);
    delete msgOLED;
    insertIntoArray(DOWNLIMITMSG, limitTemp[LOWLIMIT]);  
    display.print(msgOLED, 3, 2);
    delay(200);
    delete msgOLED;
    if(limitLevel == 0)
    {
      insertIntoArray(CHOOICEMSG, UPMSG);
      display.print(msgOLED, 4, 2);
      delay(200);
      delete msgOLED;
    }      
    else 
    {
      insertIntoArray(CHOOICEMSG, DOWNMSG);
      display.print(msgOLED, 4, 2);
      delay(200);
      delete msgOLED;
    }      
  }
  delay(1500);
}

void checkTemperature()
{
  if ((tempValue <= limitTemp[LOWLIMIT]) || (tempValue >= limitTemp[HIGHLIMIT]))
    digitalWrite(buzzer, LOW);
  else
    digitalWrite(buzzer, HIGH);
}

void increaseLimit()
{
  if(limitTemp[0] > limitTemp[1])
    limitTemp[limitLevel] += 5.0;
  if(limitTemp[0] <= limitTemp[1])
    limitTemp[limitLevel] -= 5.0;
  printLimitTemperatureMessageOnOLED(limitLevel);
  printTemperatureMessageOnOLED();
  checkTemperature();
}

void decreaseLimit()
{
  if(limitTemp[0] > limitTemp[1])
    limitTemp[limitLevel] -= 5.0;
  if(limitTemp[0] <= limitTemp[1])
    limitTemp[limitLevel] += 5.0;
  printLimitTemperatureMessageOnOLED(limitLevel);
  printTemperatureMessageOnOLED();
  checkTemperature();
}

void changeTempLimits()
{
  if(limitLevel == 0)
    limitLevel = 1;
  else
    limitLevel = 0;
  printLimitTemperatureMessageOnOLED(2);
  printTemperatureMessageOnOLED();
}

void clientIsConnected(WiFiClient client)
{ 
  String temperature = client.readStringUntil('\n'); 
  temperature.remove(temperature.length()-1);  
  Serial.println(temperature);     
  int tmp = temperature.toInt();
  tempValue = tmp/100.0; 
  printTemperatureMessageOnOLED();
  checkTemperature();
}

void loop()
{
  
  WiFiClient client = server.available();
  if (client)
  {
    if(client.connected())
    {
        clientIsConnected(client);
        
    }
    client.stop();
  }
  
}
