#pragma once

#include "../espBasicSetup.hpp"
#include "IPAddress.h"
#include <ArduinoJson.h>
#include <functional>
#include <vector>


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
	WiFi wifi;
	OTA ota;
	MQTT mqtt;
	HTTP http;
};

class ImportSetup {
  public:
	void WIFIsettings(const char *ssid, const char *pass, int mode, const char *IP, const char *subnet, const char *gateway, const char *dns1, const char *dns2);
	void WIFIsettings(const char *ssid, const char *pass, int mode);
	void OTAsettings(const char *hostname);
	void MQTTsettings(const char *broker_address, int broker_port, const char *clientID, int keepAlive, const char *willTopic, const char *willMsg, const char *user, const char *pass);
	void ServerHttpSettings(const char *user, const char *pass);

  private:
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
	bool checkJsonVariant(IPAddress &saveTo, JsonVariant IPstring);
	bool checkJsonVariant(int &saveTo, JsonVariant number);
	bool checkJsonVariant(float &saveTo, JsonVariant number);

	BasicConfig();

  private:
	std::vector<configUserHandlers::saveConfigHandler> _saveConfigHandler;
	std::vector<configUserHandlers::loadConfigHandler> _loadConfigHandler;
	size_t _userConfigSize = 0;
	void _saveUserConfig(JsonObject &userConfig);
	bool _loadUserConfig(JsonObject &userConfig);
	size_t _createConfig(ConfigData &config, String filename = "config.json", bool save = true);
	bool _loadConfig(ConfigData &config, String filename = "config.json");
	bool _inclConfig;

	friend class BasicSetup;
};

extern BasicConfig _basicConfig;
extern ConfigData _config;
