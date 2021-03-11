#ifndef ESPBASICSETUP_H_
#define ESPBASICSETUP_H_

// #define PANGO_DEBUG true

#include "espBasicSetup.hpp"


bool BasicSetup::_useLed = USE_BUILDIN_LED;

bool BasicSetup::_inclWiFi = WIFI_PLUGIN;
bool BasicSetup::_inclFS = FS_PLUGIN;
bool BasicSetup::_inclConfig = CONFIG_PLUGIN;
bool BasicSetup::_inclServerHttp = SERVERHTTP_PLUGIN;
bool BasicSetup::_inclOTA = OTA_PLUGIN;
bool BasicSetup::_inclMQTT = MQTT_PLUGIN;
bool BasicSetup::_inclTime = TIME_PLUGIN;
bool BasicSetup::_inclLogger = LOGGER_PLUGIN;

#if WIFI_PLUGIN
#if STATIC_IP
BasicWiFi WIFI(WIFI_SSID, WIFI_PASS, WIFI_MODE, WIFI_IP, WIFI_SUBNET, WIFI_GATEWAY, WIFI_DNS1, WIFI_DNS2);
#else
BasicWiFi WIFI(WIFI_SSID, WIFI_PASS, WIFI_MODE);
#endif
#endif
#if FS_PLUGIN
BasicFS basicFS;
bool BasicFS::_fsStarted = false;
#endif
#if CONFIG_PLUGIN
BasicConfig basicConfig;
ConfigData BasicConfig::configFile;
ConfigData &config = BasicConfig::configFile;    // only for cleaner sketch code
#endif
#if SERVERHTTP_PLUGIN
BasicServerHttp basicServerHttp;
AsyncWebServer &httpServer = _serverHttp;    // reference to original Web server
#endif
#if OTA_PLUGIN
BasicOTA basicOTA;
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

class EspBasicSetup {
  public:
	EspBasicSetup()
	    : config(basicConfig) {
#if OTA_PLUGIN
		_import.OTAsettings(OTA_HOST);
#endif
#if MQTT_PLUGIN
		_import.MQTTsettings(MQTT_BROKER, MQTT_BROKER_PORT, MQTT_CLIENTID, MQTT_KEEPALIVE, MQTT_WILL_TOPIC, MQTT_WILL_MSG, MQTT_USER, MQTT_PASS);
#endif
#if SERVERHTTP_PLUGIN
		_import.ServerHttpSettings(HTTP_USER, HTTP_PASS);
#endif
#if TIME_PLUGIN
		_import.timeSettings(NTP_SERVER_ADDRESS, NTP_SERVER_PORT, TIMEZONE, SUMMERTIME);
#endif
		_import.~ImportSetup();
	};
	BasicConfig &config;
	void begin() {
#if FS_PLUGIN    // mount filesystem first
		if (!(BasicFS::_fsStarted)) {
			BasicFS::_fsStarted = BasicFS::setup();
		}
#endif
#if CONFIG_PLUGIN    // after filesystem
		basicConfig.setup();
#endif
#if SERVERHTTP_PLUGIN    // after file system and before WiFi
		basicServerHttp.setup();
#endif
#if OTA_PLUGIN    // after config file and before WiFi
		basicOTA.setup();
#endif
#if MQTT_PLUGIN    // after config file and before WiFi
		MQTT.setup();
#endif
#if WIFI_PLUGIN    // after config file
		WIFI.setup(BasicWiFi::_staticIP);
#endif
#if TIME_PLUGIN    // config file and after WiFi
		NTPclient.setup();
#endif
	}

  private:
	ImportSetup _import;
};

#endif
