#ifndef ESPBASICSETUP_H_
#define ESPBASICSETUP_H_

// #define PANGO_DEBUG true

#include "espBasicSetup.hpp"


bool BasicSetup::_useLed = USE_BUILDIN_LED;

bool BasicSetup::_inclFS = FS_PLUGIN;
bool BasicSetup::_inclMQTT = MQTT_PLUGIN;
bool BasicSetup::_inclTime = TIME_PLUGIN;
bool BasicSetup::_inclLogger = LOGGER_PLUGIN;

#if FS_PLUGIN
BasicFS basicFS;
bool BasicFS::_fsStarted = false;
#endif
#if MQTT_PLUGIN
BasicMQTT MQTT;
#endif
#if TIME_PLUGIN
BasicTime NTPclient;
#endif
#if LOGGER_PLUGIN
BasicLogs logger;
#endif
ConfigData &config = _config;                // only for cleaner sketch code
BasicWiFi &WIFI = _basicWiFi;                // only for cleaner sketch code
AsyncWebServer &httpServer = _serverHttp;    // only for cleaner sketch code

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
#if MQTT_PLUGIN
		_import.MQTTsettings(MQTT_BROKER, MQTT_BROKER_PORT, MQTT_CLIENTID, MQTT_KEEPALIVE, MQTT_WILL_TOPIC, MQTT_WILL_MSG, MQTT_USER, MQTT_PASS);
#endif
		_import.ServerHttpSettings(HTTP_USER, HTTP_PASS);
#if TIME_PLUGIN
		_import.timeSettings(NTP_SERVER_ADDRESS, NTP_SERVER_PORT, TIMEZONE, SUMMERTIME);
#endif
		_import.~ImportSetup();
	};
	BasicConfig &config;
	void begin() {
		_basicSetup.begin();
#if FS_PLUGIN
    if (!(BasicFS::_fsStarted)) {
		BasicFS::_fsStarted = BasicFS::setup();
    }
#endif
#if MQTT_PLUGIN
		MQTT.setup();
#endif
#if TIME_PLUGIN
		NTPclient.setup();
#endif
	}

  private:
	ImportSetup _import;
};

#endif
