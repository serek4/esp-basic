#include "basicConfig.hpp"


ConfigData _config;
ConfigData _defaultConfig;

void ImportSetup::WIFIsettings(const char *ssid, const char *pass, int mode, const char *IP, const char *subnet, const char *gateway, const char *dns1, const char *dns2) {
	strcpy(_defaultConfig.wifi.ssid, ssid);
	strcpy(_defaultConfig.wifi.pass, pass);
	_defaultConfig.wifi.mode = mode;
	_basicSetup._staticIP = true;
	(_defaultConfig.wifi.IP).fromString(IP);
	(_defaultConfig.wifi.subnet).fromString(subnet);
	(_defaultConfig.wifi.gateway).fromString(gateway);
	(_defaultConfig.wifi.dns1).fromString(dns1);
	(_defaultConfig.wifi.dns2).fromString(dns2);
}
void ImportSetup::WIFIsettings(const char *ssid, const char *pass, int mode) {
	strcpy(_defaultConfig.wifi.ssid, ssid);
	strcpy(_defaultConfig.wifi.pass, pass);
	_defaultConfig.wifi.mode = mode;
	_basicSetup._staticIP = false;
}
void ImportSetup::OTAsettings(const char *hostname) {
	strcpy(_defaultConfig.ota.hostname, hostname);
}
void ImportSetup::ServerHttpSettings(const char *user, const char *pass) {
	strcpy(_defaultConfig.http.user, user);
	strcpy(_defaultConfig.http.pass, pass);
}
void ImportSetup::MQTTsettings(const char *broker_address, int broker_port, const char *clientID, int keepAlive, const char *willTopic, const char *willMsg, const char *user, const char *pass) {
	strcpy(_defaultConfig.mqtt.broker, broker_address);
	_defaultConfig.mqtt.broker_port = broker_port;
	strcpy(_defaultConfig.mqtt.client_ID, clientID);
	_defaultConfig.mqtt.keepalive = keepAlive;
	strcpy(_defaultConfig.mqtt.will_topic, willTopic);
	strcpy(_defaultConfig.mqtt.will_msg, willMsg);
	strcpy(_defaultConfig.mqtt.user, user);
	strcpy(_defaultConfig.mqtt.pass, pass);
}


BasicConfig::BasicConfig()
    : _inclConfig(true) {
}

void BasicConfig::setup() {
	if (!(_basicSetup._fsStarted)) {
		Serial.println("mount 1");
		_basicSetup._fsStarted = _basicFS.setup();
	}
	if (!_basicConfig._loadConfig(_config)) {
		if (!_basicConfig._loadConfig(_config, "backup-config.json")) {
			Serial.println("Loading default settings!");
			_basicConfig._createConfig(_defaultConfig);
			_config = _defaultConfig;
		}
	}
	_defaultConfig.~ConfigData();
}
void BasicConfig::saveConfig(ConfigData &config) {
	_createConfig(config);
}
void BasicConfig::loadConfig(ConfigData &config) {
	_loadConfig(config);
}
bool BasicConfig::checkJsonVariant(bool &saveTo, JsonVariant bit) {
	if (bit.is<bool>()) {
		saveTo = bit;
		return true;
	}
	return false;
}
bool BasicConfig::checkJsonVariant(char *saveTo, JsonVariant string) {
	if (string.is<char *>()) {
		strcpy(saveTo, string);
		return true;
	}
	return false;
}
bool BasicConfig::checkJsonVariant(IPAddress &saveTo, JsonVariant IPstring) {
	if (IPstring.is<const char *>()) {
		saveTo.fromString((const char *)IPstring);
		return true;
	}
	return false;
}
bool BasicConfig::checkJsonVariant(int &saveTo, JsonVariant number) {
	if (number.is<int>()) {
		saveTo = number;
		return true;
	}
	return false;
}
bool BasicConfig::checkJsonVariant(float &saveTo, JsonVariant number) {
	if (number.is<float>()) {
		saveTo = number;
		return true;
	}
	return false;
}
void BasicConfig::SetUserConfigSize(size_t size) {
	_userConfigSize = size;
}
void BasicConfig::saveUserConfig(const configUserHandlers::saveConfigHandler &handler) {
	_saveConfigHandler.push_back(handler);
}
void BasicConfig::loadUserConfig(const configUserHandlers::loadConfigHandler &handler) {
	_loadConfigHandler.push_back(handler);
}
void BasicConfig::_saveUserConfig(JsonObject &userConfig) {
	for (const auto &handler : _saveConfigHandler) handler(userConfig);
}
bool BasicConfig::_loadUserConfig(JsonObject &userConfig) {
	bool test = false;
	for (const auto &handler : _loadConfigHandler) test = handler(userConfig);
	return test;
}
bool BasicConfig::_loadConfig(ConfigData &config, String filename) {
	if (!LittleFS.exists(filename)) {
		Serial.println(filename + " not found!");
		return false;
	}
	File configFile = LittleFS.open(filename, "r");
	if (!configFile) {
		Serial.println("Failed to read " + filename + "!");
		configFile.close();
		return false;
	}
	size_t configfileSize = configFile.size();
	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(9) + 380 + _userConfigSize;
	DynamicJsonDocument doc(capacity);
	DeserializationError error = deserializeJson(doc, configFile);
	configFile.close();
	if (error) {
		Serial.println("Failed to parse " + filename + "!");
		LittleFS.rename(filename, "corrupted_" + filename);
		return false;
	}
	bool mismatch = false;
	JsonObject WiFi = doc["WiFi"];
	if (!WiFi.isNull()) {
		if (!checkJsonVariant(config.wifi.ssid, WiFi["ssid"])) mismatch |= true;    // "your-wifi-ssid"
		if (!checkJsonVariant(config.wifi.pass, WiFi["pass"])) mismatch |= true;    // "your-wifi-password"
		if (!checkJsonVariant(config.wifi.mode, WiFi["mode"])) mismatch |= true;    // "1"
		if (_basicSetup._staticIP) {
			if (!checkJsonVariant(config.wifi.IP, WiFi["IP"])) mismatch |= true;              // "192.168.0.150"
			if (!checkJsonVariant(config.wifi.subnet, WiFi["subnet"])) mismatch |= true;      // "255.255.255.0"
			if (!checkJsonVariant(config.wifi.gateway, WiFi["gateway"])) mismatch |= true;    // "192.168.0.1"
			if (!checkJsonVariant(config.wifi.dns1, WiFi["dns1"])) mismatch |= true;          // "192.168.0.1"
			if (!checkJsonVariant(config.wifi.dns2, WiFi["dns2"])) mismatch |= true;          // "1.1.1.1"
		}
	} else {
		mismatch |= true;
	}
	if (!checkJsonVariant(config.ota.hostname, doc["OTA"]["host"])) mismatch |= true;    // "esp8266-chipID"
	JsonObject MQTT = doc["MQTT"];
	if (!MQTT.isNull()) {
		if (!checkJsonVariant(config.mqtt.broker, MQTT["broker"])) mismatch |= true;              // "brocker-hostname"
		if (!checkJsonVariant(config.mqtt.broker_port, MQTT["broker_port"])) mismatch |= true;    // 1883
		if (!checkJsonVariant(config.mqtt.client_ID, MQTT["client_ID"])) mismatch |= true;        // "esp8266chipID"
		if (!checkJsonVariant(config.mqtt.keepalive, MQTT["keepalive"])) mismatch |= true;        // 15
		if (!checkJsonVariant(config.mqtt.will_topic, MQTT["will_topic"])) mismatch |= true;      // "ESP/esp8266chipID/status"
		if (!checkJsonVariant(config.mqtt.will_msg, MQTT["will_msg"])) mismatch |= true;          // "off"
		if (!checkJsonVariant(config.mqtt.user, MQTT["user"])) mismatch |= true;                  // "mqtt-user"
		if (!checkJsonVariant(config.mqtt.pass, MQTT["pass"])) mismatch |= true;                  // "mqtt-password"
	} else {
		mismatch |= true;
	}
	if (!checkJsonVariant(config.http.user, doc["HTTP"]["user"])) mismatch |= true;    // "admin"
	if (!checkJsonVariant(config.http.pass, doc["HTTP"]["pass"])) mismatch |= true;    // "admin"
	if (_userConfigSize != 0) {
		JsonObject userSettings = doc["userSettings"];
		mismatch |= !_loadUserConfig(userSettings);
	}
	if (mismatch) {
		Serial.println("Configuration in " + filename + " mismatch!");
		LittleFS.rename(filename, "mismatched_" + filename);
		return false;
	}
	Serial.println(filename + " laded!");
	if (!LittleFS.exists("backup-config.json")) {
		_createConfig(config, "backup-config.json");
	} else {
		if (filename == "backup-config.json") {
			_createConfig(config, "config.json");
		} else {
			File backup = LittleFS.open("backup-config.json", "r");
			size_t backupfileSize = backup.size();
			backup.close();
			if (configfileSize != backupfileSize) {
				_createConfig(config, "backup-config.json");
			}
		}
	}
	return true;
}
size_t BasicConfig::_createConfig(ConfigData &config, String filename, bool save) {
	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(9) + 380 + _userConfigSize;
	DynamicJsonDocument doc(capacity);

	JsonObject WiFi = doc.createNestedObject("WiFi");
	WiFi["ssid"] = config.wifi.ssid;
	WiFi["pass"] = config.wifi.pass;
	WiFi["mode"] = config.wifi.mode;
	if (_basicSetup._staticIP) {
		WiFi["IP"] = (config.wifi.IP).toString();
		WiFi["subnet"] = (config.wifi.subnet).toString();
		WiFi["gateway"] = (config.wifi.gateway).toString();
		WiFi["dns1"] = (config.wifi.dns1).toString();
		WiFi["dns2"] = (config.wifi.dns2).toString();
	}
	JsonObject OTA = doc.createNestedObject("OTA");
	OTA["host"] = config.ota.hostname;

	JsonObject MQTT = doc.createNestedObject("MQTT");
	MQTT["broker"] = config.mqtt.broker;
	MQTT["broker_port"] = config.mqtt.broker_port;
	MQTT["client_ID"] = config.mqtt.client_ID;
	MQTT["keepalive"] = config.mqtt.keepalive;
	MQTT["will_topic"] = config.mqtt.will_topic;
	MQTT["will_msg"] = config.mqtt.will_msg;
	MQTT["user"] = config.mqtt.user;
	MQTT["pass"] = config.mqtt.pass;
	JsonObject HTTP = doc.createNestedObject("HTTP");
	HTTP["user"] = config.http.user;
	HTTP["pass"] = config.http.pass;
	if (_userConfigSize != 0) {
		JsonObject userSettings = doc.createNestedObject("userSettings");
		_saveUserConfig(userSettings);
	}

	if (save) {
		File configFile = LittleFS.open(filename, "w");
		if (!configFile) {
			Serial.println("Failed to write " + filename + "!");
			configFile.close();
			return false;
		}
		serializeJsonPretty(doc, configFile);
		configFile.close();
		Serial.println(filename + " saved!");
	}
	return measureJsonPretty(doc);
}

BasicConfig _basicConfig;
