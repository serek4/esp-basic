#include "espBasicSetup.hpp"

BasicSetup _basicSetup;
BasicFS basicFS;
ConfigData _config;
BasicConfig _basicConfig;
WiFiEventHandler WiFiConnectedHandler, gotIpHandler, WiFiDisconnectedHandler;
Ticker wifiReconnectTimer;
PangolinMQTT AclientMQTT;
Ticker mqttReconnectTimer;
AsyncWebServer _serverHttp(80);


BasicSetup::BasicSetup()
    : config(_config)
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
}

BasicConfig::BasicConfig() {
	_basicSetup._inclConfig = true;
}
void BasicConfig::setup() {
	if (!(_basicSetup._fsStarted)) {
		Serial.println("mount 1");
		_basicSetup._fsStarted = basicFS.setup();
	}
	if (!_basicConfig.loadConfig()) {
		if (!_basicConfig.loadConfig("backup-config.json")) {
			Serial.println("Loading default settings!");
			_basicConfig.createConfig();
		}
	}
}
bool BasicConfig::loadConfig(String filename) {
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
	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(8) + 390;
	DynamicJsonDocument doc(capacity);
	DeserializationError error = deserializeJson(doc, configFile);
	configFile.close();
	if (error) {
		Serial.println("Failed to parse " + filename + "!");
		LittleFS.rename(filename, "corrupted_" + filename);
		return false;
	}

	size_t size = measureJsonPretty(doc);
	if (createConfig(filename, false) != size) {
		Serial.println("Configuration in " + filename + " mismatch!");
		LittleFS.rename(filename, "mismatched_" + filename);
		return false;
	}

	JsonObject WiFi = doc["WiFi"];
	strcpy(_config.wifi.ssid, WiFi["ssid"]);    // "your-wifi-ssid"
	strcpy(_config.wifi.pass, WiFi["pass"]);    // "your-wifi-password"
	_config.wifi.mode = WiFi["mode"];           // "1"
	if (_basicSetup._staticIP) {
		(_config.wifi.IP).fromString((const char *)WiFi["IP"]);              // "192.168.0.150"
		(_config.wifi.subnet).fromString((const char *)WiFi["subnet"]);      // "255.255.255.0"
		(_config.wifi.gateway).fromString((const char *)WiFi["gateway"]);    // "192.168.0.1"
		(_config.wifi.dns1).fromString((const char *)WiFi["dns1"]);          // "192.168.0.1"
		(_config.wifi.dns2).fromString((const char *)WiFi["dns2"]);          // "1.1.1.1"
	}
	strcpy(_config.ota.hostname, doc["OTA"]["host"]);    // "esp8266-chipID"

	JsonObject MQTT = doc["MQTT"];
	strcpy(_config.mqtt.broker, MQTT["broker"]);            // "brocker-hostname"
	_config.mqtt.broker_port = MQTT["broker_port"];         // 1883
	strcpy(_config.mqtt.client_ID, MQTT["client_ID"]);      // "esp8266chipID"
	_config.mqtt.keepalive = MQTT["keepalive"];             // 15
	strcpy(_config.mqtt.will_topic, MQTT["will_topic"]);    // "ESP/esp8266chipID/status"
	strcpy(_config.mqtt.will_msg, MQTT["will_msg"]);        // "off"
	strcpy(_config.mqtt.user, MQTT["user"]);                // "mqtt-user"
	strcpy(_config.mqtt.pass, MQTT["pass"]);                // "mqtt-password"
	strcpy(_config.http.user, doc["HTTP"]["user"]);         // "admin"
	strcpy(_config.http.pass, doc["HTTP"]["pass"]);         // "admin"

	Serial.println(filename + " laded!");
	if (!LittleFS.exists("backup-" + filename)) {
		createConfig("backup-" + filename);
	} else {
		File backup = LittleFS.open("backup-" + filename, "r");
		size_t backupfileSize = backup.size();
		backup.close();
		if (configfileSize != backupfileSize) {
			createConfig("backup-" + filename);
		}
	}
	return true;
}
size_t BasicConfig::createConfig(String filename, bool save) {
	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(8) + 390;
	DynamicJsonDocument doc(capacity);

	JsonObject WiFi = doc.createNestedObject("WiFi");
	WiFi["ssid"] = _config.wifi.ssid;
	WiFi["pass"] = _config.wifi.pass;
	WiFi["mode"] = _config.wifi.mode;
	if (_basicSetup._staticIP) {
		WiFi["IP"] = (_config.wifi.IP).toString();
		WiFi["subnet"] = (_config.wifi.subnet).toString();
		WiFi["gateway"] = (_config.wifi.gateway).toString();
		WiFi["dns1"] = (_config.wifi.dns1).toString();
		WiFi["dns2"] = (_config.wifi.dns2).toString();
	}
	JsonObject OTA = doc.createNestedObject("OTA");
	OTA["host"] = _config.ota.hostname;

	JsonObject MQTT = doc.createNestedObject("MQTT");
	MQTT["broker"] = _config.mqtt.broker;
	MQTT["broker_port"] = _config.mqtt.broker_port;
	MQTT["client_ID"] = _config.mqtt.client_ID;
	MQTT["keepalive"] = _config.mqtt.keepalive;
	MQTT["will_topic"] = _config.mqtt.will_topic;
	MQTT["will_msg"] = _config.mqtt.will_msg;
	MQTT["user"] = _config.mqtt.user;
	MQTT["pass"] = _config.mqtt.pass;
	JsonObject HTTP = doc.createNestedObject("HTTP");
	HTTP["user"] = _config.http.user;
	HTTP["pass"] = _config.http.pass;

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


BasicWiFi::BasicWiFi(const char *ssid, const char *pass, int mode) {
	strcpy(_config.wifi.ssid, ssid);
	strcpy(_config.wifi.pass, pass);
	_config.wifi.mode = mode;
	_basicSetup._inclWiFi = true;
	_basicSetup._staticIP = false;
}
BasicWiFi::BasicWiFi(const char *ssid, const char *pass, int mode, const char *ip, const char *subnet, const char *gateway, const char *dns1, const char *dns2) {
	strcpy(_config.wifi.ssid, ssid);
	strcpy(_config.wifi.pass, pass);
	_config.wifi.mode = mode;
	(_config.wifi.IP).fromString(ip);
	(_config.wifi.subnet).fromString(subnet);
	(_config.wifi.gateway).fromString(gateway);
	(_config.wifi.dns1).fromString(dns1);
	(_config.wifi.dns2).fromString(dns2);
	_basicSetup._staticIP = true;
	_basicSetup._inclWiFi = true;
}
void BasicWiFi::setup(bool waitForConnection) {
	if (_basicSetup._staticIP) {
		WiFi.config(_config.wifi.IP, _config.wifi.gateway, _config.wifi.subnet, _config.wifi.dns1, _config.wifi.dns2);
	}
	WiFi.mode((WiFiMode_t)_config.wifi.mode);
	WiFi.persistent(false);
	WiFi.begin(_config.wifi.ssid, _config.wifi.pass);
	WiFiConnectedHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &evt) {
		Serial.println("WiFi connected!\n SSID: " + WiFi.SSID());
	});
	gotIpHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP &evt) {
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
			mqttReconnectTimer.once(1, []() {
				AclientMQTT.connect();
			});
		}
	});
	WiFiDisconnectedHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected &evt) {
		WiFi.disconnect(true);
		Serial.println("WiFi disconnected, reconnecting!");
		if (_basicSetup._inclOTA) {
		}
		if (_basicSetup._inclMQTT) {
			mqttReconnectTimer.detach();
		}
		if (_basicSetup._inclServerHttp) {
		}
		wifiReconnectTimer.once(5, [&]() {
			WiFi.begin(_config.wifi.ssid, _config.wifi.pass);
		});
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

BasicOTA::BasicOTA() {
	sprintf(_config.ota.hostname, "esp8266-%06x", ESP.getChipId());
	_basicSetup._inclOTA = true;
};
BasicOTA::BasicOTA(const char *hostname) {
	strcpy(_config.ota.hostname, hostname);
	_basicSetup._inclOTA = true;
};
void BasicOTA::setup() {
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


BasicMQTT::BasicMQTT(const char *broker_address, int broker_port, const char *clientID, int keepAlive, const char *willTopic, const char *willMsg, const char *user, const char *pass) {
	strcpy(_config.mqtt.broker, broker_address);
	_config.mqtt.broker_port = broker_port;
	//TODO sprintf(config.mqtt.client_ID, "esp8266-%06x", ESP.getChipId());
	strcpy(_config.mqtt.client_ID, clientID);
	_config.mqtt.keepalive = keepAlive;
	strcpy(_config.mqtt.will_topic, willTopic);
	strcpy(_config.mqtt.will_msg, willMsg);
	strcpy(_config.mqtt.user, user);
	strcpy(_config.mqtt.pass, pass);
	_basicSetup._inclMQTT = true;
}
void BasicMQTT::onConnect(const MQTTuserHandlers::onMQTTconnectHandler &handler) {
	_onConnectHandler.push_back(handler);
}
void BasicMQTT::onMessage(const MQTTuserHandlers::onMQTTmesageHandler &handler) {
	_onMessageHandler.push_back(handler);
}
void BasicMQTT::publish(const char *topic, const char *payload, uint8_t qos, bool retain) {
	AclientMQTT.publish(topic, qos, retain, (uint8_t *)payload, (size_t)strlen(payload) + 1, false);
}
void BasicMQTT::publish(const char *topic, int payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	itoa(payload, numberBuffer, 10);
	AclientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer) + 1, false);
}
void BasicMQTT::publish(const char *topic, float payload, signed char width, unsigned char prec, uint8_t qos, bool retain) {
	char numberBuffer[12];
	dtostrf(payload, width, prec, numberBuffer);
	AclientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer) + 1, false);
}
uint16_t subscribe(const char *topic, uint8_t qos) {
	return AclientMQTT.subscribe(topic, qos);
}
void BasicMQTT::_onConnect() {
	Serial.println((String) "MQTT connected!\n " + AclientMQTT.getClientId() + "@" + _config.mqtt.broker);
	uint16_t subCommands = AclientMQTT.subscribe(((String) "ESP/" + AclientMQTT.getClientId() + "/commands").c_str(), 2);
	AclientMQTT.publish(((String) "ESP/" + AclientMQTT.getClientId() + "/status").c_str(), 2, true, (uint8_t *)"on", strlen("on"), false);
	for (const auto &handler : _onConnectHandler) handler();
}
void BasicMQTT::_onMessage(const char *_topic, const char *_payload) {
	for (const auto &handler : _onMessageHandler) handler(_topic, _payload);
}
void BasicMQTT::setup(bool waitForConnection) {
	AclientMQTT.setClientId(_config.mqtt.client_ID);
	AclientMQTT.setKeepAlive(_config.mqtt.keepalive);
	AclientMQTT.setWill(_config.mqtt.will_topic, 2, true, _config.mqtt.will_msg);
	AclientMQTT.setCredentials(_config.mqtt.user, _config.mqtt.pass);
	AclientMQTT.setServer(_config.mqtt.broker, _config.mqtt.broker_port);
	AclientMQTT.onConnect([&](bool sessionPresent) {
		_onConnect();
	});
	AclientMQTT.onMessage([&](const char *topic, uint8_t *payload, PANGO_PROPS_t properties, size_t len, size_t index, size_t total) {
		char fixedPayload[len + 1];
		fixedPayload[len] = '\0';
		strncpy(fixedPayload, PANGO::payloadToCstring(payload, len), len);
		_onMessage(topic, fixedPayload);
	});
	AclientMQTT.onDisconnect([](int8_t reason) {
		Serial.println((String) "MQTT disconnected: [" + (int)reason + "]!");
		if (WiFi.isConnected()) {
			mqttReconnectTimer.once(10, []() { AclientMQTT.connect(); });
		}
	});
	if (waitForConnection) {
		waitForMQTT();
	}
}
void BasicMQTT::waitForMQTT() {
	if (WiFi.status() == WL_CONNECTED) {
		int retry = 0;
		Serial.print("Connecting MQTT");
		while (!AclientMQTT.connected()) {
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

BasicServerHttp::BasicServerHttp()
    : serverHttp(_serverHttp) {
	strcpy(_config.http.user, "admin");
	strcpy(_config.http.pass, "admin");
	_basicSetup._inclServerHttp = true;
}
BasicServerHttp::BasicServerHttp(const char *user, const char *pass)
    : serverHttp(_serverHttp) {
	strcpy(_config.http.user, user);
	strcpy(_config.http.pass, pass);
	_basicSetup._inclServerHttp = true;
}
void BasicServerHttp::setup() {
	if (!(_basicSetup._fsStarted)) {
		Serial.println("mount 2");
		_basicSetup._fsStarted = basicFS.setup();
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
