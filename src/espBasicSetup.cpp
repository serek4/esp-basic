#include "espBasicSetup.hpp"

BasicSetup _basicSetup;
BasicFS _basicFS;
ConfigData _defaultConfig;
ConfigData _config;
BasicConfig _basicConfig;
BasicWiFi _basicWiFi;
BasicOTA _basicOTA;
BasicMQTT _MQTT;
BasicServerHttp _basicServerHttp;
WiFiEventHandler _WiFiConnectedHandler, _WiFiGotIpHandler, _WiFiDisconnectedHandler;
Ticker _wifiReconnectTimer;
PangolinMQTT _clientMQTT;
Ticker _mqttReconnectTimer;
AsyncWebServer _serverHttp(80);


BasicSetup::BasicSetup()
    : config(_config)
    , userConfig(_basicConfig)
    , WIFI(_basicWiFi)
    , MQTT(_MQTT)
    , serverHttp(_serverHttp)
    , _fsStarted(false)
    , _inclConfig(false)
    , _inclWiFi(false)
    , _inclOTA(false)
    , _inclMQTT(false)
    , _inclServerHttp(false) {
	Serial.begin(115200);
	Serial.println("");
}
void BasicSetup::begin() {
	_basicConfig.setup();
	_basicOTA.setup();
	_basicWiFi.setup(_config.wifi.wait, _basicSetup._staticIP);
	_basicWiFi._checkConnection();
	_MQTT.setup(_config.mqtt.wait);
	_basicServerHttp.setup();
}

void ImportSetup::WIFIsettings(const char *ssid, const char *pass, int mode, bool wait, bool staticIP, const char *IP, const char *subnet, const char *gateway, const char *dns1, const char *dns2) {
	strcpy(_defaultConfig.wifi.ssid, ssid);
	strcpy(_defaultConfig.wifi.pass, pass);
	_defaultConfig.wifi.mode = mode;
	_defaultConfig.wifi.wait = wait;
	_basicSetup._staticIP = staticIP;
	(_defaultConfig.wifi.IP).fromString(IP);
	(_defaultConfig.wifi.subnet).fromString(subnet);
	(_defaultConfig.wifi.gateway).fromString(gateway);
	(_defaultConfig.wifi.dns1).fromString(dns1);
	(_defaultConfig.wifi.dns2).fromString(dns2);
}
void ImportSetup::OTAsettings(const char *hostname) {
	strcpy(_defaultConfig.ota.hostname, hostname);
}
void ImportSetup::ServerHttpSettings(const char *user, const char *pass) {
	strcpy(_defaultConfig.http.user, user);
	strcpy(_defaultConfig.http.pass, pass);
}
void ImportSetup::MQTTsettings(const char *broker_address, int broker_port, const char *clientID, int keepAlive, bool wait, const char *willTopic, const char *willMsg, const char *user, const char *pass) {
	strcpy(_defaultConfig.mqtt.broker, broker_address);
	_defaultConfig.mqtt.broker_port = broker_port;
	strcpy(_defaultConfig.mqtt.client_ID, clientID);
	_defaultConfig.mqtt.keepalive = keepAlive;
	_defaultConfig.mqtt.wait = wait;
	strcpy(_defaultConfig.mqtt.will_topic, willTopic);
	strcpy(_defaultConfig.mqtt.will_msg, willMsg);
	strcpy(_defaultConfig.mqtt.user, user);
	strcpy(_defaultConfig.mqtt.pass, pass);
}

BasicConfig::BasicConfig() {
	_basicSetup._inclConfig = true;
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
		if (!checkJsonVariant(config.wifi.wait, WiFi["wait"])) mismatch |= true;    // true
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
		if (!checkJsonVariant(config.mqtt.wait, MQTT["wait"])) mismatch |= true;                  // true
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
	WiFi["wait"] = config.wifi.wait;
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
	MQTT["wait"] = config.mqtt.wait;
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

BasicWiFi::BasicWiFi() {
	_basicSetup._inclWiFi = true;
}
void BasicWiFi::onConnected(const WiFiUserHandlers::onWiFiConnectHandler &handler) {
	_onConnectHandler.push_back(handler);
}
void BasicWiFi::onGotIP(const WiFiUserHandlers::onWiFiGotIPhandler &handler) {
	_onGotIPhandler.push_back(handler);
}
void BasicWiFi::onDisconnected(const WiFiUserHandlers::onWiFiDisconnectHandler &handler) {
	_onDisconnectHandler.push_back(handler);
}
void BasicWiFi::_onConnected(const WiFiEventStationModeConnected &evt) {
	Serial.println("WiFi connected!\n SSID: " + WiFi.SSID());
	for (const auto &handler : _onConnectHandler) handler(evt);
}
void BasicWiFi::_onGotIP(const WiFiEventStationModeGotIP &evt) {
	Serial.println(" IP:   " + WiFi.localIP().toString());
	if (_basicSetup._inclOTA) {
		ArduinoOTA.begin();
		Serial.println("OTA started!");
	}
	if (_basicSetup._inclServerHttp) {
		_serverHttp.begin();
		Serial.println("http server started!");
	}
	if (_basicSetup._inclMQTT) {
		_mqttReconnectTimer.once(1, []() {
			_clientMQTT.connect();
		});
	}
	for (const auto &handler : _onGotIPhandler) handler(evt);
}
void BasicWiFi::_onDisconnected(const WiFiEventStationModeDisconnected &evt) {
	WiFi.disconnect(true);
	Serial.println("WiFi disconnected, reconnecting!");
	if (_basicSetup._inclOTA) {
	}
	if (_basicSetup._inclMQTT) {
		_mqttReconnectTimer.detach();
	}
	if (_basicSetup._inclServerHttp) {
	}
	_wifiReconnectTimer.once(5, [&]() {
		WiFi.begin(_config.wifi.ssid, _config.wifi.pass);
	});
	for (const auto &handler : _onDisconnectHandler) handler(evt);
}
void BasicWiFi::setup(bool waitForConnection, bool staticIP) {
	if (staticIP) {
		WiFi.config(_config.wifi.IP, _config.wifi.gateway, _config.wifi.subnet, _config.wifi.dns1, _config.wifi.dns2);
	}
	WiFi.mode((WiFiMode_t)_config.wifi.mode);
	WiFi.persistent(false);
	WiFi.begin(_config.wifi.ssid, _config.wifi.pass);
	_WiFiConnectedHandler = WiFi.onStationModeConnected([&](const WiFiEventStationModeConnected &evt) {
		_onConnected(evt);
	});
	_WiFiGotIpHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP &evt) {
		_onGotIP(evt);
	});
	_WiFiDisconnectedHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected &evt) {
		_onDisconnected(evt);
	});
	if (waitForConnection) {
		waitForWiFi();
	}
}
void BasicWiFi::waitForWiFi() {
	Serial.print("Connecting to WiFi");
	int retry = 0;
	pinMode(LED_BUILTIN, OUTPUT);
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		digitalWrite(LED_BUILTIN, LOW);
		delay(200);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(300);
		retry++;
		if (retry >= 20) {
			Serial.println("Can't connect to WiFi!");
			break;
		}
	}
}
void BasicWiFi::_checkConnection() {
	IPAddress buffer;
	Serial.print("checking DNS server");
	while (WiFi.hostByName("google.com", buffer) == 0) {
		Serial.print(".");
		delay(100);
	}
	Serial.println(" OK!");
}

BasicOTA::BasicOTA() {
	_basicSetup._inclOTA = true;
};
void BasicOTA::setup() {
	// TODO sprintf(_config.ota.hostname, "esp8266-%06x", ESP.getChipId());
	ArduinoOTA.setHostname(_config.ota.hostname);
	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else {    // U_FS
			type = "filesystem";
		}
		// NOTE: if updating FS this would be the place to unmount FS using FS.end()
		Serial.println("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) {
			Serial.println("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			Serial.println("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			Serial.println("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			Serial.println("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			Serial.println("End Failed");
		}
	});
}


BasicMQTT::BasicMQTT() {
	_basicSetup._inclMQTT = true;
}
void BasicMQTT::onConnect(const MQTTuserHandlers::onMQTTconnectHandler &handler) {
	_onConnectHandler.push_back(handler);
}
void BasicMQTT::onMessage(const MQTTuserHandlers::onMQTTmesageHandler &handler) {
	_onMessageHandler.push_back(handler);
}
void BasicMQTT::onDisconnect(const MQTTuserHandlers::onMQTTdisconnectHandler &handler) {
	_onDisconnectHandler.push_back(handler);
}
void BasicMQTT::publish(const char *topic, const char *payload, uint8_t qos, bool retain) {
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)payload, (size_t)strlen(payload) + 1, false);
}
void BasicMQTT::publish(const char *topic, int payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	itoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer) + 1, false);
}
void BasicMQTT::publish(const char *topic, float payload, signed char width, unsigned char prec, uint8_t qos, bool retain) {
	char numberBuffer[12];
	dtostrf(payload, width, prec, numberBuffer);
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer) + 1, false);
}
uint16_t subscribe(const char *topic, uint8_t qos) {
	return _clientMQTT.subscribe(topic, qos);
}
void BasicMQTT::_onConnect() {
	Serial.println((String) "MQTT connected!\n " + _clientMQTT.getClientId() + "@" + _config.mqtt.broker);
	uint16_t subStatus = _clientMQTT.subscribe(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), 2);
	uint16_t subCommands = _clientMQTT.subscribe(((String) "ESP/" + _clientMQTT.getClientId() + "/commands").c_str(), 2);
	_clientMQTT.publish(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), 2, true, (uint8_t *)"on", strlen("on"), false);
	for (const auto &handler : _onConnectHandler) handler();
}
void BasicMQTT::_onMessage(const char *_topic, const char *_payload) {
	if (strcmp(_topic, _config.mqtt.will_topic) == 0) {
		if (strcmp(_payload, _config.mqtt.will_msg) == 0) {
			_clientMQTT.publish(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), 2, true, (uint8_t *)"on", strlen("on"), false);
		}
	}
	for (const auto &handler : _onMessageHandler) handler(_topic, _payload);
}
void BasicMQTT::_onDisconnect(int8_t reason) {
	Serial.println((String) "MQTT disconnected: [" + (int)reason + "]!");
	if (WiFi.isConnected()) {
		_mqttReconnectTimer.once(10, []() { _clientMQTT.connect(); });
	}
	for (const auto &handler : _onDisconnectHandler) handler(reason);
}
void BasicMQTT::setup(bool waitForConnection) {
	//TODO sprintf(_config.mqtt.client_ID, "esp8266-%06x", ESP.getChipId());
	_clientMQTT.setClientId(_config.mqtt.client_ID);
	_clientMQTT.setKeepAlive(_config.mqtt.keepalive);
	_clientMQTT.setWill(_config.mqtt.will_topic, 2, true, _config.mqtt.will_msg);
	_clientMQTT.setCredentials(_config.mqtt.user, _config.mqtt.pass);
	_clientMQTT.setServer(_config.mqtt.broker, _config.mqtt.broker_port);
	_clientMQTT.onConnect([&](bool sessionPresent) {
		_onConnect();
	});
	_clientMQTT.onMessage([&](const char *topic, uint8_t *payload, PANGO_PROPS_t properties, size_t len, size_t index, size_t total) {
		char fixedPayload[len + 1];
		fixedPayload[len] = '\0';
		strncpy(fixedPayload, PANGO::payloadToCstring(payload, len), len);
		_onMessage(topic, fixedPayload);
	});
	_clientMQTT.onDisconnect([&](int8_t reason) {
		_onDisconnect(reason);
	});
	if (waitForConnection) {
		waitForMQTT();
	}
}
void BasicMQTT::waitForMQTT() {
	if (WiFi.status() == WL_CONNECTED) {
		int retry = 0;
		Serial.print("Connecting MQTT");
		while (!_clientMQTT.connected()) {
			Serial.print(".");
			digitalWrite(LED_BUILTIN, LOW);
			delay(100);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(150);
			retry++;
			if (retry >= 40) {
				Serial.println("Can't connect to MQTT!");
				break;
			}
		}
	}
}


BasicFS::BasicFS() {
}
bool BasicFS::setup() {
	while (!LittleFS.begin()) {
		Serial.println("LittleFS mount failed!");
		return false;
	}
	Serial.println("LittleFS mounted!");
	return true;
}

BasicServerHttp::BasicServerHttp() {
	_basicSetup._inclServerHttp = true;
}
void BasicServerHttp::setup() {
	if (!(_basicSetup._fsStarted)) {
		Serial.println("mount 2");
		_basicSetup._fsStarted = _basicFS.setup();
	}
	if (_basicSetup._fsStarted) {
		_serverHttp.addHandler(new SPIFFSEditor(_config.http.user, _config.http.pass, LittleFS));
		_serverHttp.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->redirect("/edit");
		});
		_serverHttp.onNotFound([](AsyncWebServerRequest *request) {
			String message = "File Not Found\n\n";
			message += "\nURI: ";
			message += request->url();
			message += "\nMethod: ";
			message += request->methodToString();
			message += "\nArguments: ";
			message += request->args();
			message += "\n";
			for (uint8_t i = 0; i < request->args(); i++) {
				message += " " + request->argName(i);
				message += ": " + request->arg(i) + "\n";
			}
			message += "\n";
			request->send(404, "text/plain", message);
		});
	}
}
