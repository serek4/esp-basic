#pragma once

#include "../espBasicSetup.hpp"
#include "IPAddress.h"
#include <ArduinoJson.h>
#include <StreamUtils.h>
#ifdef ARDUINO_ARCH_ESP32
#include <MD5Builder.h>
#endif
#include <functional>
#include <vector>


/** Arduino json file size
 * calculated at https://arduinojson.org/v6/assistant/
 */
#define MAIN_CONFIG_SIZE 1024

struct ConfigData {
	// WiFi settings
	struct WiFi {
		char ssid[32];
		char pass[64];
		int mode;
		IPAddress IP;         // optional
		IPAddress subnet;     // optional
		IPAddress gateway;    // optional
		IPAddress dns1;       // optional
		IPAddress dns2;       // optional
	};
	// OTA settings
	struct OTA {
		char hostname[32];
	};
	// MQTT settings
	struct MQTT {
		char broker[32];
		int broker_port;
		char client_ID[32];
		int keepalive;
		char will_topic[64];    // optional
		char will_msg[16];      // optional
		char user[16];          // optional
		char pass[16];          // optional
	};
	// web file editor
	struct HTTP {
		char user[16];
		char pass[16];
	};
	// ntp client
	struct Time {
		char NTP_server_address[32];
		int NTP_server_port;
		int timezone;
	};
	WiFi wifi;
	OTA ota;
	MQTT mqtt;
	HTTP http;
	Time time;
};

namespace configUserHandlers {
typedef std::function<void(JsonObject &userConfig)> saveConfigHandler;
typedef std::function<bool(JsonObject &userConfig)> loadConfigHandler;
}    // namespace configUserHandlers

class BasicConfig {
  public:
	void setup();
	void saveConfig(ConfigData &config);
	void loadConfig(ConfigData &config);
	void SetUserConfigSize(size_t size);
	void saveUserConfig(const configUserHandlers::saveConfigHandler &handler);
	void loadUserConfig(const configUserHandlers::loadConfigHandler &handler);
	bool checkJsonVariant(bool &saveTo, JsonVariant bit);
	bool checkJsonVariant(char *saveTo, JsonVariant string);
	bool checkJsonVariant(String saveTo, JsonVariant string);
	bool checkJsonVariant(IPAddress &saveTo, JsonVariant IPstring);
	bool checkJsonVariant(uint8_t &saveTo, JsonVariant number);
	bool checkJsonVariant(int &saveTo, JsonVariant number);
	bool checkJsonVariant(float &saveTo, JsonVariant number);
	static ConfigData configFile;

	BasicConfig();
	~BasicConfig();

  private:
	std::vector<configUserHandlers::saveConfigHandler> _saveConfigHandler;
	std::vector<configUserHandlers::loadConfigHandler> _loadConfigHandler;
	size_t _mainConfigSize;
	size_t _userConfigSize;
	void _saveUserConfig(JsonObject &userConfig);
	bool _loadUserConfig(JsonObject &userConfig);
	void _getWiFiConfig(ConfigData::WiFi &WiFiConfig);
	void _setWiFiConfig(ConfigData::WiFi &WiFiConfig);
	void _getOTAConfig(ConfigData::OTA &OTAconfig);
	void _setOTAConfig(ConfigData::OTA &OTAconfig);
	void _getServerHttpConfig(ConfigData::HTTP &HTTPconfig);
	void _setServerHttpConfig(ConfigData::HTTP &HTTPconfig);
	void _getMQTTConfig(ConfigData::MQTT &MQTTconfig);
	void _setMQTTConfig(ConfigData::MQTT &MQTTconfig);
	void _getTimeConfig(ConfigData::Time &TimeConfig);
	void _setTimeConfig(ConfigData::Time &TimeConfig);
	void _setPluginsConfigs(ConfigData &config);
	void _getPluginsConfigs(ConfigData &config);
#ifdef ARDUINO_ARCH_ESP32
	bool _writeConfigFile(ConfigData &config, String filename = "/config.json", bool save = true);
	bool _readConfigFile(ConfigData &config, String filename = "/config.json");
#elif defined(ARDUINO_ARCH_ESP8266)
	bool _writeConfigFile(ConfigData &config, String filename = "config.json", bool save = true);
	bool _readConfigFile(ConfigData &config, String filename = "config.json");
#endif
};
