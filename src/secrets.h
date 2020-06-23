#ifndef SECRETS_H_
#define SECRETS_H_
// wifi settings
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASS "your-wifi-password"
#define WIFI_MODE WIFI_STA
#define WIFI_IP "192.168.0.150"        // optional
#define WIFI_SUBNET "255.255.255.0"    // optional
#define WIFI_GATEWAY "192.168.0.1"     // optional
#define WIFI_DNS1 "192.168.0.1"        // optional
#define WIFI_DNS2 "1.1.1.1"            // optional
// OTA settings
#define OTA_HOST "esp8266wemos"    // optional
// MQTT settings
#define MQTT_BROKER "brocker-hostname"
#define MQTT_BROKER_PORT 1883
#define MQTT_CLIENTID "esp8266wemos"                      // optional
#define MQTT_KEEPALIVE 10                                 // optional
#define MQTT_WILL_TOPIC "ESP/" MQTT_CLIENTID "/status"    // optional
#define MQTT_WILL_MSG "off"                               // optional
#define MQTT_USER "mqtt-user"                             // optional
#define MQTT_PASS "mqtt-password"                         // optional
// web file editor
#define HTTP_USER "admin"
#define HTTP_PASS "admin"

#endif
