#ifndef ESPBASICSETUP_H_
#define ESPBASICSETUP_H_
#include "espBasicSetup.hpp"


BasicSetup basicSetup;
BasicOTA basicOTA(OTA_HOST);
#if STATIC_IP
BasicWiFi basicWiFi(WIFI_SSID, WIFI_PASS, WIFI_MODE, WIFI_IP, WIFI_SUBNET, WIFI_GATEWAY, WIFI_DNS1, WIFI_DNS2);
#else
BasicWiFi basicWiFi(WIFI_SSID, WIFI_PASS, WIFI_MODE);
#endif
BasicMQTT MQTT(MQTT_BROKER, MQTT_BROKER_PORT, MQTT_CLIENTID, MQTT_KEEPALIVE, MQTT_WILL_TOPIC, MQTT_WILL_MSG, MQTT_USER, MQTT_PASS);
BasicServerHttp basicServerHttp(HTTP_USER, HTTP_PASS);
AsyncWebServer &httpServer = basicServerHttp.serverHttp;    // only for cleaner sketch code

class EspBasicSetup {
  public:
	void begin() {
		basicSetup.begin();
		basicOTA.setup();
		basicServerHttp.setup();
		basicWiFi.setup(true);
		MQTT.setup(true);
	}
};

#endif