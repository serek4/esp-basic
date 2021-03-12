#pragma once

#include "../espBasicSetup.hpp"
#include "IPAddress.h"
#include <ArduinoJson.h>
#include <functional>
#include <vector>

// clang-format off
// Setup debug printing macros.
#ifdef BASIC_CONFIG_DEBUG
#define BASICCONFIG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICCONFIG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICCONFIG_PRINT(...) {}
#define BASICCONFIG_PRINTLN(...) {}
#endif
// clang-format on

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
		bool summertime;
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
	void getWiFiConfig(ConfigData::WiFi &WiFiConfig);
	void setWiFiConfig(ConfigData::WiFi &WiFiConfig);
	void getOTAConfig(ConfigData::OTA &OTAconfig);
	void setOTAConfig(ConfigData::OTA &OTAconfig);
	void getServerHttpConfig(ConfigData::HTTP &HTTPconfig);
	void setServerHttpConfig(ConfigData::HTTP &HTTPconfig);
	void getMQTTConfig(ConfigData::MQTT &MQTTconfig);
	void setMQTTConfig(ConfigData::MQTT &MQTTconfig);
	void getTimeConfig(ConfigData::Time &TimeConfig);
	void setTimeConfig(ConfigData::Time &TimeConfig);
	void saveConfig(ConfigData &config);
	void loadConfig(ConfigData &config);
	void SetUserConfigSize(size_t size);
	void saveUserConfig(const configUserHandlers::saveConfigHandler &handler);
	void loadUserConfig(const configUserHandlers::loadConfigHandler &handler);
	bool checkJsonVariant(bool &saveTo, JsonVariant bit);
	bool checkJsonVariant(char *saveTo, JsonVariant string);
	bool checkJsonVariant(IPAddress &saveTo, JsonVariant IPstring);
	bool checkJsonVariant(int &saveTo, JsonVariant number);
	bool checkJsonVariant(float &saveTo, JsonVariant number);
	static ConfigData configFile;

	BasicConfig();
	~BasicConfig();

  private:
	std::vector<configUserHandlers::saveConfigHandler> _saveConfigHandler;
	std::vector<configUserHandlers::loadConfigHandler> _loadConfigHandler;
	size_t _userConfigSize = 0;
	void _saveUserConfig(JsonObject &userConfig);
	bool _loadUserConfig(JsonObject &userConfig);
	size_t _createConfig(ConfigData &config, String filename = "config.json", bool save = true);
	bool _loadConfig(ConfigData &config, String filename = "config.json");
};
