#ifndef ESPBASICSETUP_H_
#define ESPBASICSETUP_H_

// #define PANGO_DEBUG true

#include "espBasicSetup.hpp"


bool BasicSetup::_useLed = USE_BUILDIN_LED;
ConfigData &config = _config;                // only for cleaner sketch code
BasicWiFi &WIFI = _basicWiFi;                // only for cleaner sketch code
BasicMQTT &MQTT = _basicMQTT;                // only for cleaner sketch code
AsyncWebServer &httpServer = _serverHttp;    // only for cleaner sketch code
BasicTime &NTPclient = _basicTime;           // only for cleaner sketch code

class EspBasicSetup {
  public:
	EspBasicSetup()
	    : config(_basicConfig) {
#if STATIC_IP
		_import.WIFIsettings(WIFI_SSID, WIFI_PASS, WIFI_MODE, WIFI_IP, WIFI_SUBNET, WIFI_GATEWAY, WIFI_DNS1, WIFI_DNS2);
#else
		_import.WIFIsettings(WIFI_SSID, WIFI_PASS, WIFI_MODE);
#endif
		_import.OTAsettings(OTA_HOST);
		_import.MQTTsettings(MQTT_BROKER, MQTT_BROKER_PORT, MQTT_CLIENTID, MQTT_KEEPALIVE, MQTT_WILL_TOPIC, MQTT_WILL_MSG, MQTT_USER, MQTT_PASS);
		_import.ServerHttpSettings(HTTP_USER, HTTP_PASS);
		_import.timeSettings(NTP_SERVER_ADDRESS, NTP_SERVER_PORT, TIMEZONE, SUMMERTIME);
		_import.~ImportSetup();
	};
	BasicConfig &config;
	void begin() {
		_basicSetup.begin();
	}

  private:
	ImportSetup _import;
};

#endif
