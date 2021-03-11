#include "basicConfig.hpp"


ConfigData _defaultConfig;

void ImportSetup::timeSettings(const char *NTP_server_address, int NTP_server_port, int timezone, bool summertime) {
	strcpy(_defaultConfig.time.NTP_server_address, NTP_server_address);
	_defaultConfig.time.NTP_server_port = NTP_server_port;
	_defaultConfig.time.timezone = timezone;
	_defaultConfig.time.summertime = summertime;
}


BasicConfig::BasicConfig() {
	getWiFiConfig(_defaultConfig.wifi);
	getOTAConfig(_defaultConfig.ota);
	getServerHttpConfig(_defaultConfig.http);
	getMQTTConfig(_defaultConfig.mqtt);
}
BasicConfig::~BasicConfig() {
}

void BasicConfig::getWiFiConfig(ConfigData::WiFi &WiFiConfig) {
	strcpy(WiFiConfig.ssid, BasicWiFi::_ssid);
	strcpy(WiFiConfig.pass, BasicWiFi::_pass);
	WiFiConfig.mode = static_cast<int>(BasicWiFi::_mode);
	WiFiConfig.IP = BasicWiFi::_IP;
	WiFiConfig.subnet = BasicWiFi::_subnet;
	WiFiConfig.gateway = BasicWiFi::_gateway;
	WiFiConfig.dns1 = BasicWiFi::_dns1;
	WiFiConfig.dns2 = BasicWiFi::_dns2;
}
void BasicConfig::setWiFiConfig(ConfigData::WiFi &WiFiConfig) {
	strcpy(BasicWiFi::_ssid, WiFiConfig.ssid);
	strcpy(BasicWiFi::_pass, WiFiConfig.pass);
	BasicWiFi::_mode = static_cast<WiFiMode_t>(WiFiConfig.mode);
	BasicWiFi::_IP = WiFiConfig.IP;
	BasicWiFi::_subnet = WiFiConfig.subnet;
	BasicWiFi::_gateway = WiFiConfig.gateway;
	BasicWiFi::_dns1 = WiFiConfig.dns1;
	BasicWiFi::_dns2 = WiFiConfig.dns2;
}
void BasicConfig::getOTAConfig(ConfigData::OTA &OTAconfig) {
	strcpy(OTAconfig.hostname, BasicOTA::_hostname);
}
void BasicConfig::setOTAConfig(ConfigData::OTA &OTAconfig) {
	strcpy(BasicOTA::_hostname, OTAconfig.hostname);
}
void BasicConfig::getServerHttpConfig(ConfigData::HTTP &HTTPconfig) {
	strcpy(HTTPconfig.user, BasicServerHttp::_user);
	strcpy(HTTPconfig.pass, BasicServerHttp::_pass);
}
void BasicConfig::setServerHttpConfig(ConfigData::HTTP &HTTPconfig) {
	strcpy(BasicServerHttp::_user, HTTPconfig.user);
	strcpy(BasicServerHttp::_pass, HTTPconfig.pass);
}
void BasicConfig::getMQTTConfig(ConfigData::MQTT &MQTTconfig) {
	strcpy(MQTTconfig.broker, BasicMQTT::_broker_address);
	MQTTconfig.broker_port = BasicMQTT::_broker_port;
	strcpy(MQTTconfig.client_ID, BasicMQTT::_client_ID);
	MQTTconfig.keepalive = BasicMQTT::_keepalive;
	strcpy(MQTTconfig.will_topic, BasicMQTT::_will_topic);
	strcpy(MQTTconfig.will_msg, BasicMQTT::_will_msg);
	strcpy(MQTTconfig.user, BasicMQTT::_user);
	strcpy(MQTTconfig.pass, BasicMQTT::_pass);
}
void BasicConfig::setMQTTConfig(ConfigData::MQTT &MQTTconfig) {
	strcpy(BasicMQTT::_broker_address, MQTTconfig.broker);
	BasicMQTT::_broker_port = MQTTconfig.broker_port;
	strcpy(BasicMQTT::_client_ID, MQTTconfig.client_ID);
	BasicMQTT::_keepalive = MQTTconfig.keepalive;
	strcpy(BasicMQTT::_will_topic, MQTTconfig.will_topic);
	strcpy(BasicMQTT::_will_msg, MQTTconfig.will_msg);
	strcpy(BasicMQTT::_user, MQTTconfig.user);
	strcpy(BasicMQTT::_pass, MQTTconfig.pass);
}
void BasicConfig::setup() {
	if (!(BasicFS::_fsStarted)) {
		BASICFS_PRINTLN("mount 1");
		BasicFS::_fsStarted = BasicFS::setup();
	}
	if (!_loadConfig(configFile)) {
		if (!_loadConfig(configFile, "backup-config.json")) {
			BASICCONFIG_PRINTLN("Loading default settings!");
			if (BasicSetup::_inclLogger) {
				BasicLogs::saveLog(now(), ll_error, "loaded default settings!");
			}
			_createConfig(_defaultConfig);
			configFile = _defaultConfig;
		}
	}
	_defaultConfig.~ConfigData();
	setWiFiConfig(configFile.wifi);
	setOTAConfig(configFile.ota);
	setServerHttpConfig(configFile.http);
	setMQTTConfig(configFile.mqtt);
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
		BASICCONFIG_PRINTLN(filename + " not found!");
		return false;
	}
	File configFile = LittleFS.open(filename, "r");
	if (!configFile) {
		BASICCONFIG_PRINTLN("Failed to read " + filename + "!");
		configFile.close();
		return false;
	}
	size_t configfileSize = configFile.size();
	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2 * JSON_OBJECT_SIZE(8) + 450 + _userConfigSize;
	DynamicJsonDocument doc(capacity);
	DeserializationError error = deserializeJson(doc, configFile);
	configFile.close();
	if (error) {
		BASICCONFIG_PRINTLN("Failed to parse " + filename + "!");
		if (BasicSetup::_inclLogger) {
			BasicLogs::saveLog(now(), ll_warning, filename + " corrupted");
		}
		LittleFS.rename(filename, "corrupted_" + filename);
		return false;
	}
	bool mismatch = false;
	JsonObject WiFi = doc["WiFi"];
	if (!WiFi.isNull()) {
		if (!checkJsonVariant(config.wifi.ssid, WiFi["ssid"])) mismatch |= true;    // "your-wifi-ssid"
		if (!checkJsonVariant(config.wifi.pass, WiFi["pass"])) mismatch |= true;    // "your-wifi-password"
		if (!checkJsonVariant(config.wifi.mode, WiFi["mode"])) mismatch |= true;    // "1"
		if (BasicWiFi::_staticIP) {
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
	if (BasicSetup::_inclTime) {
		JsonObject time = doc["time"];
		if (!time.isNull()) {
			if (!checkJsonVariant(config.time.NTP_server_address, time["NTP_server_address"])) mismatch |= true;    // "router.lan"
			if (!checkJsonVariant(config.time.NTP_server_port, time["NTP_server_port"])) mismatch |= true;          // 2390
			if (!checkJsonVariant(config.time.timezone, time["timezone"])) mismatch |= true;                        // 1
			if (!checkJsonVariant(config.time.summertime, time["summertime"])) mismatch |= true;                    // false
		} else {
			mismatch |= true;
		}
	}
	if (_userConfigSize != 0) {
		JsonObject userSettings = doc["userSettings"];
		mismatch |= !_loadUserConfig(userSettings);
	}
	if (mismatch) {
		BASICCONFIG_PRINTLN("Configuration in " + filename + " mismatch!");
		if (BasicSetup::_inclLogger) {
			BasicLogs::saveLog(now(), ll_warning, "config mismatch in " + filename);
		}
		LittleFS.rename(filename, "mismatched_" + filename);
		return false;
	}
	BASICCONFIG_PRINTLN(filename + " laded!");
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_log, filename + " laded");
	}
	if (!LittleFS.exists("backup-config.json")) {
		_createConfig(config, "backup-config.json");
		if (BasicSetup::_inclLogger) {
			BasicLogs::saveLog(now(), ll_log, "config backup file saved");
		}
	} else {
		if (filename == "backup-config.json") {
			_createConfig(config, "config.json");
			if (BasicSetup::_inclLogger) {
				BasicLogs::saveLog(now(), ll_warning, "config restored from backup file");
			}
		} else {
			File backup = LittleFS.open("backup-config.json", "r");
			size_t backupfileSize = backup.size();
			backup.close();
			if (configfileSize != backupfileSize) {
				_createConfig(config, "backup-config.json");
				if (BasicSetup::_inclLogger) {
					BasicLogs::saveLog(now(), ll_log, "config backup file updated");
				}
			}
		}
	}
	return true;
}
size_t BasicConfig::_createConfig(ConfigData &config, String filename, bool save) {
	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2 * JSON_OBJECT_SIZE(8) + 450 + _userConfigSize;
	DynamicJsonDocument doc(capacity);

	JsonObject WiFi = doc.createNestedObject("WiFi");
	WiFi["ssid"] = config.wifi.ssid;
	WiFi["pass"] = config.wifi.pass;
	WiFi["mode"] = config.wifi.mode;
	if (BasicWiFi::_staticIP) {
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
	if (BasicSetup::_inclTime) {
		JsonObject time = doc.createNestedObject("time");
		time["NTP_server_address"] = config.time.NTP_server_address;
		time["NTP_server_port"] = config.time.NTP_server_port;
		time["timezone"] = config.time.timezone;
		time["summertime"] = config.time.summertime;
	}
	if (_userConfigSize != 0) {
		JsonObject userSettings = doc.createNestedObject("userSettings");
		_saveUserConfig(userSettings);
	}

	if (save) {
		File configFile = LittleFS.open(filename, "w");
		if (!configFile) {
			BASICCONFIG_PRINTLN("Failed to write " + filename + "!");
			configFile.close();
			return false;
		}
		serializeJsonPretty(doc, configFile);
		configFile.close();
		BASICCONFIG_PRINTLN(filename + " saved!");
	}
	return measureJsonPretty(doc);
}

BasicConfig _basicConfig;
