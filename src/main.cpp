#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

const char *ssid = "arif";         // your network SSID (name)
const char *password = "anik1234"; // your network password

WiFiClient client;

unsigned long myChannelNumber = 2226123;
const char *myWriteAPIKey = "TIUK9843SYH97WWE";

#define sensorIn 13
#define sensorOut 14

int count = 0;
int in, out;

// #include <string.h>
unsigned long previous = 0;
unsigned long timeDelay = 10000;

Adafruit_SSD1306 oled = Adafruit_SSD1306(128, 64, &Wire);

// put function declarations here:

void oledWrite(String txt, int x, int y, int k)
{
  // oled.clearDisplay();

  oled.setTextSize(k);
  oled.setTextColor(WHITE);
  oled.setCursor(x, y);
  oled.println(txt);
  oled.display();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(sensorIn, INPUT_PULLUP);
  pinMode(sensorOut, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client); // Initialize ThingSpeak

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect1");
    oled.clearDisplay();
    oledWrite("connecting...", 0, 0, 2);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, password);
      delay(5000);
    }
    oled.clearDisplay();
    oledWrite("No student", 0, 0, 2);
    Serial.println("\nConnected.");
  }
  // delay(100);
}

void loop()
{

  if ((millis() - previous) > timeDelay)
  {
    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED)
    {
      oled.clearDisplay();
      oledWrite("connecting...", 0, 0, 2);
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      oled.clearDisplay();
      // oledWrite("Total", 0, 0);
      oledWrite(String(count), 10, 0, 9);

      Serial.println("\nConnected.");
    }

    ThingSpeak.setField(1, count);
    ThingSpeak.setStatus("myStatus");
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    // uncomment if you want to get temperature in Fahrenheit
    // int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);

    if (x == 200)
    {
      Serial.println("Channel update successful.");
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    previous = millis();
  }

  in = digitalRead(sensorIn);
  out = digitalRead(sensorOut);

  if (in == 1 && out == 0)
  {
    while (1)
    {
      int flag = 0;
      in = digitalRead(sensorIn);
      out = digitalRead(sensorOut);
      // if (in == 1 && out == 1)
      if (out == 1)
      {
        while (1)
        {
          in = digitalRead(sensorIn);
          out = digitalRead(sensorOut);
          if (in == 0 && out == 0)
          {
            count++;
            flag = 1;
            Serial.println(count);
            oled.clearDisplay();
            // oledWrite("Total", 0, 0);
            oledWrite(String(count), 10, 0, 9);
            break;
          }
          else{
            delay(10);
          }
        }
      }
      else
      {
        delay(10);
      }
      if (flag == 1)
      {
        flag = 0;
        break;
      }
    }
  }
  if (in == 0 && out == 1)
  {
    while (1)
    {
      int flag = 0;
      in = digitalRead(sensorIn);
      out = digitalRead(sensorOut);
      // if (in == 1 && out == 1)
      if (in == 1)
      {
        while (1)
        {
          in = digitalRead(sensorIn);
          out = digitalRead(sensorOut);
          if (in == 0 && out == 0)
          {
            count--;
            if (count < 1)
            {
              count = 0;
              flag = 1;
              Serial.println(count);
              oled.clearDisplay();
              oledWrite("No student", 0, 0, 2);
              break;
            }
            flag = 1;
            Serial.println(count);
            oled.clearDisplay();
            // oledWrite("Total", 0, 0);
            oledWrite(String(count), 10, 0, 9);
            break;
          }
          else{
            delay(10);
          }
        }
      }
      else
      {
        delay(10);
      }
      if (flag == 1)
      {
        flag = 0;
        break;
      }
    }
  }
}
