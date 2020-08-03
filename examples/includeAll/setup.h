#include "secrets.h"
#include <espBasicSetup.h>

#ifndef SETUP_H_
#define SETUP_H_


espBasicSetup _setup;
BasicOTA OTA(OTA_HOST);
#if STATIC_IP
BasicWiFi basicWiFi(WIFI_SSID, WIFI_PASS, WIFI_MODE, WIFI_IP, WIFI_SUBNET, WIFI_GATEWAY, WIFI_DNS1, WIFI_DNS2);
#else
BasicWiFi basicWiFi(WIFI_SSID, WIFI_PASS, WIFI_MODE);
#endif
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
