#ifndef ESPBASICSETUP_H_
#define ESPBASICSETUP_H_
#include "espBasicSetup.hpp"


BasicSetup basicSetup;
ConfigData &config = basicSetup.config;                // only for cleaner sketch code
BasicWiFi &WIFI = basicSetup.WIFI;                     // only for cleaner sketch code
BasicMQTT &MQTT = basicSetup.MQTT;                     // only for cleaner sketch code
AsyncWebServer &httpServer = basicSetup.serverHttp;    // only for cleaner sketch code

class EspBasicSetup {
  public:
	EspBasicSetup()
	    : config(basicSetup.userConfig) {
		_import.WIFIsettings(WIFI_SSID, WIFI_PASS, WIFI_MODE, STATIC_IP, WIFI_IP, WIFI_SUBNET, WIFI_GATEWAY, WIFI_DNS1, WIFI_DNS2);
		_import.OTAsettings(OTA_HOST);
		_import.MQTTsettings(MQTT_BROKER, MQTT_BROKER_PORT, MQTT_CLIENTID, MQTT_KEEPALIVE, MQTT_WILL_TOPIC, MQTT_WILL_MSG, MQTT_USER, MQTT_PASS);
		_import.ServerHttpSettings(HTTP_USER, HTTP_PASS);
		_import.~ImportSetup();
	};
	BasicConfig &config;
	void begin() {
		basicSetup.begin();
	}

  private:
	ImportSetup _import;
};

#endif