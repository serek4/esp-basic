#include "basicConfig.hpp"


ConfigData _defaultConfig;

BasicConfig::BasicConfig()
    : _userConfigSize(0)
    , _mainConfigSize(MAIN_CONFIG_SIZE) {
	_getPluginsConfigs(_defaultConfig);
}
BasicConfig::~BasicConfig() {
}

void BasicConfig::_getWiFiConfig(ConfigData::WiFi &WiFiConfig) {
	strcpy(WiFiConfig.ssid, BasicWiFi::_ssid);
	strcpy(WiFiConfig.pass, BasicWiFi::_pass);
	WiFiConfig.mode = static_cast<int>(BasicWiFi::_mode);
	WiFiConfig.IP = BasicWiFi::_IP;
	WiFiConfig.subnet = BasicWiFi::_subnet;
	WiFiConfig.gateway = BasicWiFi::_gateway;
	WiFiConfig.dns1 = BasicWiFi::_dns1;
	WiFiConfig.dns2 = BasicWiFi::_dns2;
}
void BasicConfig::_setWiFiConfig(ConfigData::WiFi &WiFiConfig) {
	strcpy(BasicWiFi::_ssid, WiFiConfig.ssid);
	strcpy(BasicWiFi::_pass, WiFiConfig.pass);
	BasicWiFi::_mode = static_cast<WiFiMode_t>(WiFiConfig.mode);
	BasicWiFi::_IP = WiFiConfig.IP;
	BasicWiFi::_subnet = WiFiConfig.subnet;
	BasicWiFi::_gateway = WiFiConfig.gateway;
	BasicWiFi::_dns1 = WiFiConfig.dns1;
	BasicWiFi::_dns2 = WiFiConfig.dns2;
}
void BasicConfig::_getOTAConfig(ConfigData::OTA &OTAconfig) {
	strcpy(OTAconfig.hostname, BasicOTA::_hostname);
}
void BasicConfig::_setOTAConfig(ConfigData::OTA &OTAconfig) {
	strcpy(BasicOTA::_hostname, OTAconfig.hostname);
}
void BasicConfig::_getServerHttpConfig(ConfigData::HTTP &HTTPconfig) {
	strcpy(HTTPconfig.user, BasicServerHttp::_user);
	strcpy(HTTPconfig.pass, BasicServerHttp::_pass);
}
void BasicConfig::_setServerHttpConfig(ConfigData::HTTP &HTTPconfig) {
	strcpy(BasicServerHttp::_user, HTTPconfig.user);
	strcpy(BasicServerHttp::_pass, HTTPconfig.pass);
}
void BasicConfig::_getMQTTConfig(ConfigData::MQTT &MQTTconfig) {
	strcpy(MQTTconfig.broker, BasicMQTT::_broker_address);
	MQTTconfig.broker_port = BasicMQTT::_broker_port;
	strcpy(MQTTconfig.client_ID, BasicMQTT::_client_ID);
	MQTTconfig.keepalive = BasicMQTT::_keepalive;
	strcpy(MQTTconfig.will_topic, BasicMQTT::_will_topic);
	strcpy(MQTTconfig.will_msg, BasicMQTT::_will_msg);
	strcpy(MQTTconfig.user, BasicMQTT::_user);
	strcpy(MQTTconfig.pass, BasicMQTT::_pass);
}
void BasicConfig::_setMQTTConfig(ConfigData::MQTT &MQTTconfig) {
	strcpy(BasicMQTT::_broker_address, MQTTconfig.broker);
	BasicMQTT::_broker_port = MQTTconfig.broker_port;
	strcpy(BasicMQTT::_client_ID, MQTTconfig.client_ID);
	BasicMQTT::_keepalive = MQTTconfig.keepalive;
	strcpy(BasicMQTT::_will_topic, MQTTconfig.will_topic);
	strcpy(BasicMQTT::_will_msg, MQTTconfig.will_msg);
	strcpy(BasicMQTT::_user, MQTTconfig.user);
	strcpy(BasicMQTT::_pass, MQTTconfig.pass);
}
void BasicConfig::_getTimeConfig(ConfigData::Time &TimeConfig) {
	strcpy(TimeConfig.NTP_server_address, BasicTime::_NTP_server_address);
	TimeConfig.NTP_server_port = BasicTime::_NTP_server_port;
	TimeConfig.timezone = BasicTime::_timezone;
}
void BasicConfig::_setTimeConfig(ConfigData::Time &TimeConfig) {
	strcpy(BasicTime::_NTP_server_address, TimeConfig.NTP_server_address);
	BasicTime::_NTP_server_port = TimeConfig.NTP_server_port;
	BasicTime::_timezone = TimeConfig.timezone;
}
void BasicConfig::_getPluginsConfigs(ConfigData &config) {
	_getWiFiConfig(config.wifi);
	_getOTAConfig(config.ota);
	_getServerHttpConfig(config.http);
	_getMQTTConfig(config.mqtt);
	_getTimeConfig(config.time);
}
void BasicConfig::_setPluginsConfigs(ConfigData &config) {
	_setWiFiConfig(config.wifi);
	_setOTAConfig(config.ota);
	_setServerHttpConfig(config.http);
	_setMQTTConfig(config.mqtt);
	_setTimeConfig(config.time);
}
void BasicConfig::setup() {
	if (!(BasicFS::_fsStarted)) {
		BASICFS_PRINTLN("mount 1");
		BasicFS::_fsStarted = BasicFS::setup();
	} else {
		BASICFS_PRINTLN("mount 1 skipped");
	}
	if (!_readConfigFile(configFile)) {
		if (!_readConfigFile(configFile, "backup-config.json")) {
			BASICCONFIG_PRINTLN("Loading default settings!");
			if (BasicSetup::_inclLogger) {
				BasicLogs::saveLog(now(), ll_error, "loaded default settings!");
			}
			_writeConfigFile(_defaultConfig);
			configFile = _defaultConfig;
		}
	}
	_defaultConfig.~ConfigData();
	_setPluginsConfigs(configFile);
}
void BasicConfig::saveConfig(ConfigData &config) {
	_writeConfigFile(config);
	_setPluginsConfigs(config);
}
void BasicConfig::loadConfig(ConfigData &config) {
	_readConfigFile(config);
	_setPluginsConfigs(config);
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
bool BasicConfig::checkJsonVariant(String saveTo, JsonVariant string) {
	if (string.is<String>()) {
		saveTo = string.as<String>();
		return true;
	}
	return false;
}
bool BasicConfig::checkJsonVariant(uint8_t &saveTo, JsonVariant number) {
	if (number.is<uint8_t>()) {
		saveTo = number;
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
bool BasicConfig::_readConfigFile(ConfigData &config, String filename) {
	if (!FILE_SYSTEM.exists(BasicFS::fileName(filename))) {
		BASICCONFIG_PRINTLN(filename + " not found!");
		return false;
	}
	File configFile = FILE_SYSTEM.open(BasicFS::fileName(filename), "r");
	if (!configFile) {
		BASICCONFIG_PRINTLN("Failed to read " + filename + "!");
		configFile.close();
		return false;
	}
#ifdef ARDUINO_ARCH_ESP32
	MD5Builder _md5;
	_md5.begin();
	_md5.add(configFile.readString());
	_md5.calculate();
	String configfileMd5 = _md5.toString();    // get config.json md5
#elif defined(ARDUINO_ARCH_ESP8266)
	String configfileSha = sha1(configFile.readString());    // get config.json sha1
#endif
	configFile.seek(0, SeekSet);    // return to file begining
	size_t capacity = _mainConfigSize + _userConfigSize;
	DynamicJsonDocument doc(capacity);
	ReadBufferingStream bufferedConfigFile(configFile, 64);
	DeserializationError error = deserializeJson(doc, bufferedConfigFile);
	configFile.close();
	if (error) {
		BASICCONFIG_PRINTLN("Failed to parse " + filename + "!");
		if (BasicSetup::_inclLogger) {
			BasicLogs::saveLog(now(), ll_warning, filename + " corrupted");
		}
		FILE_SYSTEM.rename(BasicFS::fileName(filename), BasicFS::fileName("corrupted_" + filename));
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
		FILE_SYSTEM.rename(BasicFS::fileName(filename), BasicFS::fileName("mismatched_" + filename));
		return false;
	}
	BASICCONFIG_PRINTLN(filename + " laded!");
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_log, filename + " laded");
	}
	if (!FILE_SYSTEM.exists(BasicFS::fileName("backup-config.json"))) {
		_writeConfigFile(config, BasicFS::fileName("backup-config.json"));
		if (BasicSetup::_inclLogger) {
			BasicLogs::saveLog(now(), ll_log, "config backup file saved");
		}
	} else {
		if (filename == BasicFS::fileName("backup-config.json")) {
			_writeConfigFile(config, BasicFS::fileName("config.json"));
			if (BasicSetup::_inclLogger) {
				BasicLogs::saveLog(now(), ll_warning, "config restored from backup file");
			}
		} else {
#ifdef ARDUINO_ARCH_ESP32
			File backup = FILE_SYSTEM.open(BasicFS::fileName("backup-config.json"), "r");
			MD5Builder _md5;
			_md5.begin();
			_md5.add(backup.readString());
			_md5.calculate();
			String backupfileMd5 = _md5.toString();
			BASICCONFIG_PRINT("config md5: ");
			BASICCONFIG_PRINTLN(configfileMd5);
			BASICCONFIG_PRINT("backup md5: ");
			BASICCONFIG_PRINTLN(backupfileMd5);
			backup.close();
			if (configfileMd5 != backupfileMd5) {
				_writeConfigFile(config, BasicFS::fileName("backup-config.json"));
#elif defined(ARDUINO_ARCH_ESP8266)
			File backup = FILE_SYSTEM.open("backup-config.json", "r");
			String backupfileSha = sha1(backup.readString());
			BASICCONFIG_PRINT("config sha1: ");
			BASICCONFIG_PRINTLN(configfileSha);
			BASICCONFIG_PRINT("backup sha1: ");
			BASICCONFIG_PRINTLN(backupfileSha);
			backup.close();
			if (configfileSha != backupfileSha) {
				_writeConfigFile(config, "backup-config.json");
#endif
				if (BasicSetup::_inclLogger) {
					BasicLogs::saveLog(now(), ll_log, "config backup file updated");
				}
			}
		}
	}
	return true;
}
bool BasicConfig::_writeConfigFile(ConfigData &config, String filename, bool save) {
	size_t capacity = _mainConfigSize + _userConfigSize;
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
	}
	if (_userConfigSize != 0) {
		JsonObject userSettings = doc.createNestedObject("userSettings");
		_saveUserConfig(userSettings);
	}

	if (save) {
		bool fileExist = FILE_SYSTEM.exists(BasicFS::fileName(filename));
		File configFile = FILE_SYSTEM.open(BasicFS::fileName(filename), "w");
		if (!configFile) {
			BASICCONFIG_PRINTLN("Failed to write " + filename + "!");
			configFile.close();
			return false;
		}
		WriteBufferingStream bufferedConfigFile(configFile, 64);
		serializeJsonPretty(doc, bufferedConfigFile);
		bufferedConfigFile.flush();
		configFile.close();
		BASICCONFIG_PRINTLN(filename + (fileExist ? " updated!" : " saved!"));
	}
	return true;
}
