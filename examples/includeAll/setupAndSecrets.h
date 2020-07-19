#include <espBasicSetup.h>

#ifndef SETUPANDSECRETS_H_
#define SETUPANDSECRETS_H_

#define STATIC_IP true
#define MQTT_SET_LASTWILL true
#define MQTT_USE_CREDENTIALS true

// wifi settings
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASS "your-wifi-password"
#define WIFI_MODE 1
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
#define MQTT_WILL_TOPIC "ESP/wemos/status"    // optional (%s = MQTT_CLIENTID)
#define MQTT_WILL_MSG "off"                   // optional
#endif
#if MQTT_USE_CREDENTIALS
#define MQTT_USER "mqtt-user"        // optional
#define MQTT_PASS "mqtt-password"    // optional
#endif
// web file editor
#define HTTP_USER "admin"
#define HTTP_PASS "admin"


espBasicSetup _setup;
BasicOTA OTA(OTA_HOST);
BasicWiFi basicWiFi(WIFI_SSID, WIFI_PASS, WIFI_MODE, WIFI_IP, WIFI_SUBNET, WIFI_GATEWAY, WIFI_DNS1, WIFI_DNS2);
BasicMQTT MQTT(MQTT_BROKER, MQTT_BROKER_PORT, MQTT_CLIENTID, MQTT_KEEPALIVE, MQTT_WILL_TOPIC, MQTT_WILL_MSG, MQTT_USER, MQTT_PASS);
BasicFileEditor basicFileEditor(HTTP_USER, HTTP_PASS);

class LocalSetup {
  public:
	void begin() {
		_setup.begin();
		OTA.setup();
		basicFileEditor.setup();
		basicWiFi.setup(true);
		MQTT.setup(true);
	}
};

#endif
