#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_INTERRUPT_RETRY_COUNT 0

#include <Arduino.h>
#include <ESP8266WiFi.h>  //if you get an error here you need to install the ESP8266 board manager
#include <ESP8266mDNS.h>  //if you get an error here you need to install the ESP8266 board manager
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <ArduinoOTA.h>   //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
#include <SimpleTimer.h>  //https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include "FastLED.h"

#include <SwartNinjaRSW.h>
#include <SwartNinjaLED.h>

#include "user_config.h" // Fixed user configurable options
#ifdef USE_CONFIG_OVERRIDE
#include "user_config_override.h" // Configuration overrides for my_user_config.h
#endif

///////////////////////////////////////////////////////////////////////////
//   General Declarations
///////////////////////////////////////////////////////////////////////////

char ESP_CHIP_ID[7] = {0};
char OTA_HOSTNAME[sizeof(ESP_CHIP_ID) + sizeof(OTA_HOSTNAME_TEMPLATE) - 2] = {0};

///////////////////////////////////////////////////////////////////////////
//   WiFi
///////////////////////////////////////////////////////////////////////////
// function declaration
void setupWiFi(void);
void connectWiFi(void);
void onConnected(const WiFiEventStationModeConnected &event);
void onDisconnect(const WiFiEventStationModeDisconnected &event);
void onGotIP(const WiFiEventStationModeGotIP &event);
void loopWiFiSensor(void);
int getWiFiSignalStrength(void);

// variables declaration
int previousWiFiSignalStrength = -1;
unsigned long previousMillis = 0;
int reqConnect = 0;
int isConnected = 0;
const long interval = 500;
const long reqConnectNum = 15; // number of intervals to wait for connection
WiFiEventHandler mConnectHandler;
WiFiEventHandler mDisConnectHandler;
WiFiEventHandler mGotIpHandler;

// Initialize the Ethernet mqttClient object
WiFiClient wifiClient;

/* -------------------------------------------------
 *  MQTT
 * ------------------------------------------------- */
// function declaration
void setupMQTT();
void connectToMQTT();
void checkInMQTT();
void subscribeToMQTT(char *p_topic);
void publishToMQTT(char *p_topic, char *p_payload, bool retain = true);
void handleMQTTMessage(char *topic, byte *payload, unsigned int length);

// variables declaration
bool boot;
char MQTT_CHAR_PAYLOAD[50];
char MQTT_PAYLOAD[8] = {0};
char MQTT_AVAILABILITY_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_AVAILABILITY_TOPIC_TEMPLATE) - 2] = {0};
char MQTT_WIFI_QUALITY_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_SENSOR_TOPIC_TEMPLATE) + sizeof(WIFI_QUALITY_SENSOR_NAME) - 4] = {0};

// main light
char MQTT_MAIN_LIGHT_STATE_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_MAIN_LIGHT_STATE_TOPIC_TEMPLATE) - 2] = {0};
char MQTT_MAIN_LIGHT_CMND_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_MAIN_LIGHT_CMND_TOPIC_TEMPLATE) - 2] = {0};
char MQTT_MAIN_LIGHT_BRIGHTNESS_STATE_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_MAIN_LIGHT_BRIGHTNESS_STATE_TEMPLATE) - 2] = {0};
char MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TEMPLATE) - 2] = {0};
// main light
char MQTT_COLOR_LIGHT_POWER_STATE_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_COLOR_LIGHT_POWER_STATE_TEMPLATE) - 2] = {0};
char MQTT_COLOR_LIGHT_POWER_CMND_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_COLOR_LIGHT_POWER_CMND_TEMPLATE) - 2] = {0};
char MQTT_COLOR_LIGHT_BRIGHTNESS_STATE_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_COLOR_LIGHT_BRIGHTNESS_STATE_TEMPLATE) - 2] = {0};
char MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TEMPLATE) - 2] = {0};
char MQTT_COLOR_LIGHT_COLOR_STATE_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_COLOR_LIGHT_COLOR_STATE_TEMPLATE) - 2] = {0};
char MQTT_COLOR_LIGHT_COLOR_CMND_TOPIC[sizeof(ESP_CHIP_ID) + sizeof(MQTT_COLOR_LIGHT_COLOR_CMND_TEMPLATE) - 2] = {0};

// Initialize the mqtt mqttClient object
PubSubClient mqttClient(wifiClient);

///////////////////////////////////////////////////////////////////////////
// SwartNinjaLED
///////////////////////////////////////////////////////////////////////////
// function declaration
void publishLightBrightness(void);
void publishLightState(void);

// variables declaration
bool stripStateChanged = false;

// Initialize the LED object
SwartNinjaLED mainLight(MAIN_LIGHT_PIN);

///////////////////////////////////////////////////////////////////////////
//   FastLED
///////////////////////////////////////////////////////////////////////////
// function declaration
void chooseColorLightPattern();

bool colorLightsPower = false;
byte brightness = 255;
String effect = "None";
byte current_red = 0;
byte current_green = 0;
byte current_blue = 0;

// Initialize the mqtt FastLED object
CRGB colorLight[COLOR_LIGHT_NUM_LEDS];

///////////////////////////////////////////////////////////////////////////
//  SwartNinjaRSW
///////////////////////////////////////////////////////////////////////////
// function declaration
void handleSwartNinjaSensorUpdate(char *value, int pin, const char *event);
// initialize the SwartNinjaRSW object
SwartNinjaRSW lightSwitch(LIGHT_SWITCH_PIN, handleSwartNinjaSensorUpdate, false);

///////////////////////////////////////////////////////////////////////////
//   SimpleTimer
///////////////////////////////////////////////////////////////////////////
SimpleTimer timer;

///////////////////////////////////////////////////////////////////////////
//  MAIN SETUP AND LOOP
///////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  boot == true;
  // Set the chip ID
  sprintf(ESP_CHIP_ID, "%06X", ESP.getChipId());

  // WIFI
  setupWiFi();

  // MQTT
  setupMQTT();

  // Over the air
  sprintf(OTA_HOSTNAME, OTA_HOSTNAME_TEMPLATE, ESP_CHIP_ID);
  Serial.print("AOT Hostname: ");
  Serial.println(OTA_HOSTNAME);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.begin();

  delay(10);

  // light switch setup
  lightSwitch.setup();

  mainLight.setup();

  // setup color light
  FastLED.addLeds<COLOR_LIGHT_LED_TYPE, COLOR_LIGHT_PIN, COLOR_LIGHT_COLOR_ORDER>(colorLight, COLOR_LIGHT_NUM_LEDS);
  FastLED.setDither(0);

  timer.setInterval(120000, checkInMQTT);
}

void loop()
{
  // WIFI
  connectWiFi();

  // Code will only run if connected to WiFi
  if (isConnected == 2)
  {
    // MQTT
    if (!mqttClient.connected())
    {
      connectToMQTT();
    }
    mqttClient.loop();

    // Over the air
    if (!colorLightsPower)
    {
      ArduinoOTA.handle();
    }

    // Check WiFi signal
    loopWiFiSensor();

    // color light
    chooseColorLightPattern();
    FastLED.show(brightness);

    timer.run();
  }

  // light switch loop
  lightSwitch.loop();
}

///////////////////////////////////////////////////////////////////////////
//  SwartNinjaRSW
///////////////////////////////////////////////////////////////////////////
void handleSwartNinjaSensorUpdate(char *value, int pin, const char *event)
{
  if (event == SN_RSW_SENSOR_EVT)
  {
    bool mainLightState = !mainLight.getRawState();
    if (mainLight.setState(mainLightState))
    {
      publishToMQTT(MQTT_MAIN_LIGHT_STATE_TOPIC, mainLight.getState());
    }
  }
}

///////////////////////////////////////////////////////////////////////////
//   FastLED
///////////////////////////////////////////////////////////////////////////
void chooseColorLightPattern()
{
  if (colorLightsPower)
  {
    if (effect == "SomeEffect")
    {
      // need to create effects.
    }
    else
    {
      fill_solid(colorLight, COLOR_LIGHT_NUM_LEDS, CRGB(current_red, current_green, current_blue));
    }
  }
  else
  {
    fill_solid(colorLight, COLOR_LIGHT_NUM_LEDS, CRGB::Black);
  }
}

///////////////////////////////////////////////////////////////////////////
//   WiFi
///////////////////////////////////////////////////////////////////////////

/*
 * Function called to setup WiFi module
 */
void setupWiFi(void)
{
  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  mConnectHandler = WiFi.onStationModeConnected(onConnected);
  mDisConnectHandler = WiFi.onStationModeDisconnected(onDisconnect);
  mGotIpHandler = WiFi.onStationModeGotIP(onGotIP);
}

/*
 * Function called to connect to WiFi
 */
void connectWiFi(void)
{
  if (WiFi.status() != WL_CONNECTED && reqConnect > reqConnectNum && isConnected < 2)
  {
    reqConnect = 0;
    isConnected = 0;
    WiFi.disconnect();

    Serial.println();
    Serial.print("[WIFI]: Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_SSID);

    WiFi.hostname(WIFI_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("[WIFI]: Connecting...");
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    reqConnect++;
  }
}

/*
 * Function called to handle WiFi events
 */
void onConnected(const WiFiEventStationModeConnected &event)
{
  char macAdddress[20];
  sprintf(macAdddress, "%02X:%02X:%02X:%02X:%02X:%02X", event.bssid[5], event.bssid[4], event.bssid[3], event.bssid[2], event.bssid[1], event.bssid[0]);
  Serial.print(F("[WIFI]: You're connected to the AP. (MAC - "));
  Serial.print(macAdddress);
  Serial.println(")");
  isConnected = 1;
}

void onDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("[WIFI]: Disconnected");
  Serial.print("[WIFI]: Reason: ");
  Serial.println(event.reason);
  isConnected = 0;
}

void onGotIP(const WiFiEventStationModeGotIP &event)
{
  Serial.print("[WIFI]: IP Address : ");
  Serial.println(event.ip);
  Serial.print("[WIFI]: Subnet     : ");
  Serial.println(event.mask);
  Serial.print("[WIFI]: Gateway    : ");
  Serial.println(event.gw);

  isConnected = 2;
}

/*
 * Function to check WiFi signal strength
 */
void loopWiFiSensor(void)
{
  static unsigned long lastWiFiQualityMeasure = 0;
  if (lastWiFiQualityMeasure + WIFI_QUALITY_INTERVAL <= millis() || previousWiFiSignalStrength == -1)
  {
    lastWiFiQualityMeasure = millis();
    int currentWiFiSignalStrength = getWiFiSignalStrength();
    if (isnan(previousWiFiSignalStrength) || currentWiFiSignalStrength <= previousWiFiSignalStrength - WIFI_QUALITY_OFFSET_VALUE || currentWiFiSignalStrength >= previousWiFiSignalStrength + WIFI_QUALITY_OFFSET_VALUE)
    {
      previousWiFiSignalStrength = currentWiFiSignalStrength;
      dtostrf(currentWiFiSignalStrength, 2, 2, MQTT_PAYLOAD);
      publishToMQTT(MQTT_WIFI_QUALITY_TOPIC, MQTT_PAYLOAD);
    }
  }
}

/*
 * Helper function to get the current WiFi signal strength
 */
int getWiFiSignalStrength(void)
{
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

///////////////////////////////////////////////////////////////////////////
//   MQTT
///////////////////////////////////////////////////////////////////////////

/*
 * Function called to setup MQTT topics
 */
void setupMQTT()
{
  sprintf(MQTT_AVAILABILITY_TOPIC, MQTT_AVAILABILITY_TOPIC_TEMPLATE, ESP_CHIP_ID);
  Serial.println();
  Serial.println("---------------------------------------------------------------------------");
  Serial.print(F("[MQTT]: MQTT availability topic: "));
  Serial.println(MQTT_AVAILABILITY_TOPIC);

  sprintf(MQTT_WIFI_QUALITY_TOPIC, MQTT_SENSOR_TOPIC_TEMPLATE, ESP_CHIP_ID, WIFI_QUALITY_SENSOR_NAME);
  Serial.print(F("[MQTT]: MQTT WiFi Quality topic: "));
  Serial.println(MQTT_WIFI_QUALITY_TOPIC);
  Serial.println("---------------------------------------------------------------------------");

  sprintf(MQTT_MAIN_LIGHT_STATE_TOPIC, MQTT_MAIN_LIGHT_STATE_TOPIC_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Main Light State topic: "));
  Serial.println(MQTT_MAIN_LIGHT_STATE_TOPIC);

  sprintf(MQTT_MAIN_LIGHT_CMND_TOPIC, MQTT_MAIN_LIGHT_CMND_TOPIC_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Main Light Command topic: "));
  Serial.println(MQTT_MAIN_LIGHT_CMND_TOPIC);

  sprintf(MQTT_MAIN_LIGHT_BRIGHTNESS_STATE_TOPIC, MQTT_MAIN_LIGHT_BRIGHTNESS_STATE_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Main Light Brightness State topic: "));
  Serial.println(MQTT_MAIN_LIGHT_BRIGHTNESS_STATE_TOPIC);

  sprintf(MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TOPIC, MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Main Light Brightness Command topic: "));
  Serial.println(MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TOPIC);
  Serial.println("---------------------------------------------------------------------------");

  sprintf(MQTT_COLOR_LIGHT_POWER_STATE_TOPIC, MQTT_COLOR_LIGHT_POWER_STATE_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Color Light power State topic: "));
  Serial.println(MQTT_COLOR_LIGHT_POWER_STATE_TOPIC);

  sprintf(MQTT_COLOR_LIGHT_POWER_CMND_TOPIC, MQTT_COLOR_LIGHT_POWER_CMND_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Color Light power Command topic: "));
  Serial.println(MQTT_COLOR_LIGHT_POWER_CMND_TOPIC);

  sprintf(MQTT_COLOR_LIGHT_BRIGHTNESS_STATE_TOPIC, MQTT_COLOR_LIGHT_BRIGHTNESS_STATE_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Color Light Brightness State topic: "));
  Serial.println(MQTT_COLOR_LIGHT_BRIGHTNESS_STATE_TOPIC);

  sprintf(MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TOPIC, MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Color Light Brightness Command topic: "));
  Serial.println(MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TOPIC);

  sprintf(MQTT_COLOR_LIGHT_COLOR_STATE_TOPIC, MQTT_COLOR_LIGHT_COLOR_STATE_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Color Light color State topic: "));
  Serial.println(MQTT_COLOR_LIGHT_COLOR_STATE_TOPIC);

  sprintf(MQTT_COLOR_LIGHT_COLOR_CMND_TOPIC, MQTT_COLOR_LIGHT_COLOR_CMND_TEMPLATE, ESP_CHIP_ID);
  Serial.print(F("[MQTT]: MQTT Color Light color Command topic: "));
  Serial.println(MQTT_COLOR_LIGHT_COLOR_CMND_TOPIC);
  Serial.println("---------------------------------------------------------------------------");

  mqttClient.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
  mqttClient.setCallback(handleMQTTMessage);
}

void handleMQTTMessage(char *topic, byte *payload, unsigned int length)
{
  String newTopic = topic;
  payload[length] = '\0';
  String newPayload = String((char *)payload);

  Serial.print("[MQTT]: handleMQTTMessage - Message arrived, topic: ");
  Serial.print(topic);
  Serial.print(", payload: ");
  Serial.println(newPayload);
  Serial.println();
  newPayload.toCharArray(MQTT_CHAR_PAYLOAD, newPayload.length() + 1);

  if (newTopic == MQTT_MAIN_LIGHT_CMND_TOPIC)
  {
    if (mainLight.setState(newPayload.equalsIgnoreCase(MQTT_PAYLOAD_ON)))
    {
      publishToMQTT(MQTT_MAIN_LIGHT_STATE_TOPIC, mainLight.getState());
    }
  }
  else if (String(MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TOPIC).equals(newTopic))
  {
    // test if the payload is equal to "ON" or "OFF"
    if (mainLight.setBrightness(newPayload.toInt()))
    {
      publishToMQTT(MQTT_MAIN_LIGHT_BRIGHTNESS_STATE_TOPIC, mainLight.getBrightness());
    }
  }
  else if (String(MQTT_COLOR_LIGHT_POWER_CMND_TOPIC).equals(newTopic))
  {
    colorLightsPower = String(newPayload).equals(MQTT_PAYLOAD_ON);
    publishToMQTT(MQTT_COLOR_LIGHT_POWER_STATE_TOPIC, MQTT_CHAR_PAYLOAD);
  }
  else if (String(MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TOPIC).equals(newTopic))
  {
    brightness = newPayload.toInt();
    publishToMQTT(MQTT_COLOR_LIGHT_POWER_STATE_TOPIC, MQTT_CHAR_PAYLOAD);
  }
  else if (String(MQTT_COLOR_LIGHT_COLOR_CMND_TOPIC).equals(newTopic))
  {
    publishToMQTT(MQTT_COLOR_LIGHT_COLOR_STATE_TOPIC, MQTT_CHAR_PAYLOAD);

    // get the position of the first and second commas
    uint8_t firstIndex = newPayload.indexOf(',');
    uint8_t lastIndex = newPayload.lastIndexOf(',');

    uint8_t red = newPayload.substring(0, firstIndex).toInt();
    uint8_t green = newPayload.substring(firstIndex + 1, lastIndex).toInt();
    uint8_t blue = newPayload.substring(lastIndex + 1).toInt();
    if ((red < 0 || red > 255) || (green < 0 || green > 255) || (blue < 0 || blue > 255))
    {
      return;
    }
    // update current color values
    current_red = red;
    current_green = green;
    current_blue = blue;
  }
}

/*
  Function called to connect/reconnect to the MQTT broker
*/
void connectToMQTT()
{
  int retries = 0;
  // Loop until we're connected / reconnected
  while (!mqttClient.connected())
  {
    if (retries < 150)
    {
      Serial.println("[MQTT]: Attempting MQTT connection...");
      if (mqttClient.connect(ESP_CHIP_ID, MQTT_USERNAME, MQTT_PASSWORD, MQTT_AVAILABILITY_TOPIC, 0, 1, "offline"))
      {

        Serial.println(F("[MQTT]: The mqttClient is successfully connected to the MQTT broker"));
        publishToMQTT(MQTT_AVAILABILITY_TOPIC, "online");
        if (boot == false)
        {
          Serial.println(F("[MQTT]: Reconnected"));
        }
        if (boot == true)
        {
          Serial.println(F("[MQTT]: Rebooted"));
          boot == false;
        }

        // publish messages
        // subscribeToMQTT(MQTT_COLOR_LIGHT_POWER_STATE_TOPIC);
        // subscribeToMQTT(MQTT_COLOR_LIGHT_COLOR_STATE_TOPIC);

        // ... and resubscribe
        subscribeToMQTT(MQTT_MAIN_LIGHT_CMND_TOPIC);
        subscribeToMQTT(MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TOPIC);
        subscribeToMQTT(MQTT_COLOR_LIGHT_POWER_CMND_TOPIC);
        subscribeToMQTT(MQTT_COLOR_LIGHT_COLOR_CMND_TOPIC);
        subscribeToMQTT(MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TOPIC);
      }
      else
      {
        retries++;
#ifdef DEBUG
        Serial.println(F("[MQTT]: ERROR - The connection to the MQTT broker failed"));
        Serial.print(F("[MQTT]: MQTT username: "));
        Serial.println(MQTT_USERNAME);
        Serial.print(F("[MQTT]: MQTT password: "));
        Serial.println(MQTT_PASSWORD);
        Serial.print(F("[MQTT]: MQTT broker: "));
        Serial.println(MQTT_SERVER);
        Serial.print(F("[MQTT]: Retries: "));
        Serial.println(retries);
#endif
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    if (retries > 149)
    {
      ESP.restart();
    }
  }
}

void checkInMQTT()
{
  publishToMQTT(MQTT_AVAILABILITY_TOPIC, "online", false);
  timer.setTimeout(120000, checkInMQTT);
}

/*
  Function called to subscribe to a MQTT topic
*/
void subscribeToMQTT(char *p_topic)
{
  if (mqttClient.subscribe(p_topic))
  {
    Serial.print(F("[MQTT]: subscribeToMQTT - Sending the MQTT subscribe succeeded for topic: "));
    Serial.println(p_topic);
  }
  else
  {
    Serial.print(F("[MQTT]: subscribeToMQTT - ERROR, Sending the MQTT subscribe failed for topic: "));
    Serial.println(p_topic);
  }
}

/*
  Function called to publish to a MQTT topic with the given payload
*/
void publishToMQTT(char *p_topic, char *p_payload, bool retain)
{
  if (mqttClient.publish(p_topic, p_payload, retain))
  {
    Serial.print(F("[MQTT]: publishToMQTT - MQTT message published successfully, topic: "));
    Serial.print(p_topic);
    Serial.print(F(", payload: "));
    Serial.println(p_payload);
  }
  else
  {
    Serial.println(F("[MQTT]: publishToMQTT - ERROR, MQTT message not published, either connection lost, or message too large. Topic: "));
    Serial.print(p_topic);
    Serial.print(F(" , payload: "));
    Serial.println(p_payload);
  }
}
