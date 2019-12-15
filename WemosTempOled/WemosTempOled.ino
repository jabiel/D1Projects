// Oled
#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// internet time NTP
#include <WiFiUdp.h>
#include <NTPClient.h>

// Wifi
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// Wifi password
#include "../../secrets.h"

// Temp
#include <OneWire.h>
#include <DallasTemperature.h>

#define DEBUG

#ifdef DEBUG
#  define DEBUG_LOG(x) Serial.print(x)
#else
#  define DEBUG_LOG(x)
#endif

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// DS temp
#define ONE_WIRE_DS_TEMP_PIN D4
OneWire oneWire(ONE_WIRE_DS_TEMP_PIN);
DallasTemperature sensors(&oneWire);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, 3600);

String CurrentTime = "test";
String CurrentTemp = "99.9";

void setup()   {
  Serial.begin(9600);
  Serial.println("Starting up...");

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.display();
  SetupWifi(SECRET_SSID, SECRET_PASS);
  
  //InitSettings();
  timeClient.begin();
  delay(1000);
  display.clearDisplay();
  display.display();
}

int interval = 28;
void loop() {
  //iotWebConf.doLoop();// for settings
  if (interval >= 30)
  {
    timeClient.update();
    CurrentTime = timeClient.getFormattedTime();
    Serial.println("Time: " + CurrentTime);

    sensors.requestTemperatures();
    CurrentTemp = String(sensors.getTempCByIndex(0));
    Serial.println(CurrentTemp.substring(0, 2));
    Serial.println(CurrentTemp.substring(2, 4));

    interval = 0;
  }

  oledPrintAll(CurrentTemp.substring(0, 2), CurrentTemp.substring(2, 4), CurrentTime);
  interval++;
  delay(1000);
  DEBUG_LOG(".");
}

void SetupWifi(String ssid, String pass)
{
  WiFi.mode(WIFI_STA);
  WiFi.hostname("WemosOled");
  WiFi.begin(ssid, pass);
  DEBUG_LOG("WiFi connecting: ");
  while (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_LOG(".");
    delay(500);
  }
  DEBUG_LOG("Connected. Local IP: ");
  DEBUG_LOG(WiFi.localIP());
}

void oledPrintAll(String temp1, String temp2, String time1)
{
  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setTextSize(3);
  display.setCursor(0, 0);
  display.print(temp1);

  display.setTextSize(2);
  display.setCursor(34, 7);
  display.print(temp2);

  display.setTextSize(2);
  display.setCursor(0, 32);
  display.print(time1);

  display.display();
}
