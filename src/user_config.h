#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

///////////////////////////////////////////////////////////////////////////
//   WIFI
///////////////////////////////////////////////////////////////////////////
#define WIFI_SSID "wifi_ssid"
#define WIFI_PASSWORD "wifi_password"
#define WIFI_HOSTNAME "SmartFishTank"
#define WIFI_QUALITY_OFFSET_VALUE 2
#define WIFI_QUALITY_INTERVAL 50000 // [ms]
#define WIFI_QUALITY_SENSOR_NAME "wifi"

///////////////////////////////////////////////////////////////////////////
//   MQTT
///////////////////////////////////////////////////////////////////////////
#define MQTT_SERVER "xxx.xxx.xxx.xxx"
#define MQTT_SERVER_PORT 1883
#define MQTT_USERNAME "mqtt_user_name"
#define MQTT_PASSWORD "mqtt_password"

#define MQTT_AVAILABILITY_TOPIC_TEMPLATE "%s/status" // MQTT availability: online/offline
#define MQTT_SENSOR_TOPIC_TEMPLATE "%s/sensor/%s"
#define MQTT_STATE_TOPIC_TEMPLATE "%s/%s/state"
#define MQTT_COMMAND_TOPIC_TEMPLATE "cmnd/%s/%s"

#define MQTT_MAIN_LIGHT_STATE_TOPIC_TEMPLATE "%s/main-light/power"
#define MQTT_MAIN_LIGHT_CMND_TOPIC_TEMPLATE "cmnd/%s/main-light/power"
#define MQTT_MAIN_LIGHT_BRIGHTNESS_STATE_TEMPLATE "%s/main-light/brightness"
#define MQTT_MAIN_LIGHT_BRIGHTNESS_CMND_TEMPLATE "cmnd/%s/main-light/brightness"

#define MQTT_COLOR_LIGHT_POWER_STATE_TEMPLATE "%s/color-light/power"
#define MQTT_COLOR_LIGHT_POWER_CMND_TEMPLATE "cmnd/%s/color-light/power"
#define MQTT_COLOR_LIGHT_BRIGHTNESS_STATE_TEMPLATE "%s/color-light/brightness"
#define MQTT_COLOR_LIGHT_BRIGHTNESS_CMND_TEMPLATE "cmnd/%s/color-light/brightness"
#define MQTT_COLOR_LIGHT_COLOR_STATE_TEMPLATE "%s/color-light/color"
#define MQTT_COLOR_LIGHT_COLOR_CMND_TEMPLATE "cmnd/%s/color-light/color"

#define MQTT_PAYLOAD_ON "ON"
#define MQTT_PAYLOAD_OFF "OFF"

/* --------------------------------------------------------------------------------------------------
 * FastLED
 * -------------------------------------------------------------------------------------------------- */
#define COLOR_LIGHT_NUM_LEDS 20
#define COLOR_LIGHT_LED_TYPE WS2811
#define COLOR_LIGHT_COLOR_ORDER BRG //Color orders, can be RGB, RBG, GRB, GBR, BRG, BGR

///////////////////////////////////////////////////////////////////////////
//   PINS
///////////////////////////////////////////////////////////////////////////
#define MAIN_LIGHT_PIN D1
#define COLOR_LIGHT_PIN 4 // pin D2 on nodemcu
#define LIGHT_SWITCH_PIN D7

#define SNSW_DEBUG
///////////////////////////////////////////////////////////////////////////
//   Over-the-Air update (OTA)
///////////////////////////////////////////////////////////////////////////
#define OTA_HOSTNAME_TEMPLATE WIFI_HOSTNAME "_%s"
#define OTA_PORT 8266 // port 8266 by default

#endif // _USER_CONFIG_H_