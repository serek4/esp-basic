#ifndef SECRETS_H_
#define SECRETS_H_

#define USE_BUILDIN_LED true
#define STATIC_IP false
#define MQTT_SET_LASTWILL true
#define MQTT_USE_CREDENTIALS true

// wifi settings
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASS "your-wifi-password"
#define WIFI_MODE WIFI_STA
#if STATIC_IP
#define WIFI_IP "192.168.0.150"        // optional
#define WIFI_SUBNET "255.255.255.0"    // optional
#define WIFI_GATEWAY "192.168.0.1"     // optional
#define WIFI_DNS1 "192.168.0.1"        // optional
#define WIFI_DNS2 "1.1.1.1"            // optional
#endif
// OTA settings
#define OTA_HOST "esp8266-wemos"    // optional
// MQTT settings
#define MQTT_BROKER "broker-hostname"
#define MQTT_BROKER_PORT 1883
#define MQTT_CLIENTID "wemos"
#define MQTT_KEEPALIVE 15
#if MQTT_SET_LASTWILL
#define MQTT_WILL_TOPIC "ESP/wemos/status"    // optional
#define MQTT_WILL_MSG "off"                   // optional
#endif
#if MQTT_USE_CREDENTIALS
#define MQTT_USER "mqtt-user"        // optional
#define MQTT_PASS "mqtt-password"    // optional
#endif
// web file editor
#define HTTP_USER "admin"
#define HTTP_PASS "admin"
// ntp client settings
#define NTP_SERVER_ADDRESS "0.pool.ntp.org"
#define NTP_SERVER_PORT 2390
#define TIMEZONE 1    // Central European Time (Europe/Warsaw)
#define SUMMERTIME false
// user custom settings
#define USER_STR "teststr"
#define USER_INT 15

#endif
