#ifndef SECRETS_H_
#define SECRETS_H_

#define USE_LED true
#if USE_LED
#define LED_PIN LED_BUILTIN
#define LED_ON HIGH
#endif

#define WIFI_PLUGIN true
#define FS_PLUGIN true
#define CONFIG_PLUGIN true
#define SERVERHTTP_PLUGIN true
#define OTA_PLUGIN true
#define MQTT_PLUGIN true
#define TIME_PLUGIN true
#define LOGGER_PLUGIN true

#if WIFI_PLUGIN
#define STATIC_IP false
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
#endif
// OTA settings
#if OTA_PLUGIN
#define OTA_HOSTNAME "esp32-devkit"    // optional
#endif
// MQTT settings
#if MQTT_PLUGIN
#define MQTT_SET_LASTWILL true
#define MQTT_USE_CREDENTIALS true
#define MQTT_BROKER "broker-hostname"
#define MQTT_BROKER_PORT 1883
#define MQTT_CLIENTID "devkit"
#define MQTT_KEEPALIVE 15
#if MQTT_SET_LASTWILL
#define MQTT_WILL_TOPIC "esp/devkit/status"    // optional
#define MQTT_WILL_MSG "0"                      // optional
#endif
#if MQTT_USE_CREDENTIALS
#define MQTT_USER "mqtt-user"        // optional
#define MQTT_PASS "mqtt-password"    // optional
#endif
#endif
// web file editor
#if SERVERHTTP_PLUGIN
#define HTTP_USER "admin"
#define HTTP_PASS "admin"
#endif
// ntp client settings
#if TIME_PLUGIN
#define NTP_SERVER_ADDRESS "0.pool.ntp.org"
#define NTP_SERVER_PORT 2390
#define TIMEZONE 1    // Central European Time (Europe/Warsaw)
#endif
// user custom settings
#define USER_CONFIG_SIZE 96
#define USER_STR "teststr"
#define USER_INT 15

#endif
