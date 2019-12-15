// https://github.com/prampec/IotWebConf/blob/master/examples/IotWebConf06MqttApp/IotWebConf06MqttApp.ino
#include <IotWebConf.h>

#define STRING_LEN 128
// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "mqt1"
// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN D2

// -- Status indicator pin.
#define STATUS_PIN LED_BUILTIN

void wifiConnected();
void configSaved();
void handleRoot();
const char thingName[] = "jareksThing";

char mqttServerValue[STRING_LEN];
char mqttUserNameValue[STRING_LEN];
char mqttUserPasswordValue[STRING_LEN];

DNSServer dnsServer;
WebServer server(80);
HTTPUpdateServer httpUpdater;
WiFiClient net;


IotWebConf iotWebConf(thingName, &dnsServer, &server, "", CONFIG_VERSION);
IotWebConfParameter mqttServerParam = IotWebConfParameter("MQTT server", "mqttServer", mqttServerValue, STRING_LEN);
IotWebConfParameter mqttUserNameParam = IotWebConfParameter("MQTT user", "mqttUser", mqttUserNameValue, STRING_LEN);
IotWebConfParameter mqttUserPasswordParam = IotWebConfParameter("MQTT password", "mqttPass", mqttUserPasswordValue, STRING_LEN, "password");
boolean needMqttConnect = false;
boolean needReset = false;
void InitSettings()
{
  iotWebConf.setStatusPin(STATUS_PIN);
  iotWebConf.setConfigPin(CONFIG_PIN);
  iotWebConf.addParameter(&mqttServerParam);
  iotWebConf.addParameter(&mqttUserNameParam);
  iotWebConf.addParameter(&mqttUserPasswordParam);
  iotWebConf.setConfigSavedCallback(&configSaved);
  //iotWebConf.setFormValidator(&formValidator);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  iotWebConf.setupUpdateServer(&httpUpdater);

  // -- Initializing the configuration.
  boolean validConfig = iotWebConf.init();
  if (!validConfig)
  {
    mqttServerValue[0] = '\0';
    mqttUserNameValue[0] = '\0';
    mqttUserPasswordValue[0] = '\0';
  }

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });
}

void LoopSettings()
{
  iotWebConf.doLoop();
  if (needMqttConnect)
  {
    //if (connectMqtt())
    //{
    //  
    //}
    needMqttConnect = false;
  }
  else if (iotWebConf.getState() == IOTWEBCONF_STATE_ONLINE) // && (!mqttClient.connected()))
  {
    Serial.println("MQTT reconnect");
    //connectMqtt();
  }

  if (needReset)
  {
    Serial.println("Rebooting after 1 second.");
    iotWebConf.delay(1000);
    ESP.restart();
  }

  //unsigned long now = millis();
  //if ((500 < now - lastReport) && (pinState != digitalRead(CONFIG_PIN)))
  //{
  //  pinState = 1 - pinState; // invert pin state as it is changed
  //  lastReport = now;
 //   Serial.print("Sending on MQTT channel '/test/status' :");
  //  Serial.println(pinState == LOW ? "ON" : "OFF");
  //  //mqttClient.publish("/test/status", pinState == LOW ? "ON" : "OFF");
  //}  
}

void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 06 MQTT App</title></head><body>MQTT App demo";
  s += "<ul>";
  s += "<li>MQTT server: ";
  s += mqttServerValue;
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void wifiConnected()
{
  needMqttConnect = true;
}

void configSaved()
{
  Serial.println("Configuration was updated.");
  needReset = true;
}
