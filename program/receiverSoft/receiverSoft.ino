#include <OLED.h>
#include <ESP8266WiFi.h>

//server data
const char* ssid = "Kontroler_temp";
const char* password = "kontroler_haslo";
WiFiServer server(80);
WiFiClient client;

//pins
const int buzzer = 10;
const int choose_button = 11;
const int up_button = 12;
const int down_button = 13;
const int sda = 2;
const int scl = 14;

//program
OLED display(sda, scl);
const int HIGH_LIMIT = 0;
const int LOW_LIMIT = 1;
float limit_temp[] = {60, 30};
String temperature = "";
float temp_value = 0;
const String head_of_msg = "Temperatura: ";
String msg = "";
char* msgOLED = "";
int delay_value = 200;

void setupPins()
{
  pinMode(buzzer, OUTPUT);
  pinMode(choose_button, INPUT);
  pinMode(up_button, INPUT);
  pinMode(down_button, INPUT);
  digitalWrite(buzzer, LOW);
  attachInterrupt(digitalPinToInterrupt(choose_button), changeTempLimits, RISING);
}

void setupOLED()
{
   display.begin();
   display.print("Witaj!");
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.begin();

}

void readTemperatureValue()
{
  temperature = client.readStringUntil('\r');
  temp_value = temperature.toFloat();
}

//dodać wyświetlanie na OLED
void printTemperatureMessageOnOLED()
{
  msg = String(head_of_msg) + temp_value
        + "\nG: " + limit_temp[HIGH_LIMIT] + "\nD: " + limit_temp[LOW_LIMIT];
  
  msg.toCharArray(msgOLED, msg.length());
  display.clear();
  display.print(msgOLED);
}

//dodać wyświetlanie na OLED
void printLimitTemperatureMessageOnOLED(int up)
{
  if (up == 0)
    msg =  String("G: ") + limit_temp[HIGH_LIMIT];
  else
    msg = String("D: ") + limit_temp[LOW_LIMIT];

  msg.toCharArray(msgOLED, msg.length());
  display.clear();
  display.print(msgOLED);
}
void checkTemperature()
{
  if ((temp_value <= limit_temp[LOW_LIMIT]) || (temp_value >= limit_temp[HIGH_LIMIT]))
    digitalWrite(buzzer, HIGH);
  else
    digitalWrite(buzzer, LOW);
}

void changeTempLimitsValue(int limit_edge)
{
  if ((digitalRead(up_button) == HIGH) && (digitalRead(down_button) == LOW))
  {
    limit_temp[limit_edge] += 5;
    delay(delay_value);
  }
  else if ((digitalRead(up_button) == LOW) && (digitalRead(down_button) == HIGH))
  {
    limit_temp[limit_edge] -= 5;
    delay(delay_value);
  }
}

void changeTempLimits()
{
  detachInterrupt(choose_button);
  int up = 0;
  boolean loop_state = true;
  int state = 0;
  printLimitTemperatureMessageOnOLED(up);

  while (loop_state)
  {
    state = digitalRead(choose_button);
    if (state == HIGH)
    {
      up = up++;
      delay(delay_value);
    }

    if (up < 2)
    {
      changeTempLimitsValue(up);
      printLimitTemperatureMessageOnOLED(up);
      break;
    }
    else
    {
      loop_state = false;
      break;
    }

  }
  printTemperatureMessageOnOLED();
  checkTemperature();
  attachInterrupt(digitalPinToInterrupt(choose_button), changeTempLimits, RISING);
}

void clientConnected()
{
  readTemperatureValue();
  printTemperatureMessageOnOLED();
  checkTemperature();
}

void loop()
{
  client = server.available();
  if (client)
  {
    clientConnected();
  }


}
