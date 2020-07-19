#include "espBasicSetup.hpp"

SharedSetup basicSetup;
BasicFS basicFS;
BasicConfig config;
WiFiEventHandler WiFiConnectedHandler, gotIpHandler, WiFiDisconnectedHandler;
Ticker wifiReconnectTimer;
PangolinMQTT AclientMQTT;
Ticker mqttReconnectTimer;
AsyncWebServer editorServer(80);

void espBasicSetup::begin() {
	config.setup();
}

SharedSetup::SharedSetup()
    : _fsStarted(false)
    , _inclConfig(false)
    , _inclWiFi(false)
    , _inclOTA(false)
    , _inclMQTT(false)
    , _inclWebEditor(false) {

	Serial.begin(115200);
	Serial.println("");
}

BasicConfig::BasicConfig() {
	basicSetup._inclConfig = true;
}
void BasicConfig::setup() {
	if (!(basicSetup._fsStarted)) {
		Serial.println("mount 1");
		basicSetup._fsStarted = basicFS.setup();
	}
	if (!config.loadConfig()) {
		if (!config.loadConfig("backup-config.json")) {
			Serial.println("Loading default settings!");
			config.createConfig();
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
	strcpy(config.wifi.ssid, WiFi["ssid"]);    // "your-wifi-ssid"
	strcpy(config.wifi.pass, WiFi["pass"]);    // "your-wifi-password"
	config.wifi.mode = WiFi["mode"];           // "1"
	if (basicSetup._staticIP) {
		(config.wifi.IP).fromString((const char *)WiFi["IP"]);              // "192.168.0.150"
		(config.wifi.subnet).fromString((const char *)WiFi["subnet"]);      // "255.255.255.0"
		(config.wifi.gateway).fromString((const char *)WiFi["gateway"]);    // "192.168.0.1"
		(config.wifi.dns1).fromString((const char *)WiFi["dns1"]);          // "192.168.0.1"
		(config.wifi.dns2).fromString((const char *)WiFi["dns2"]);          // "1.1.1.1"
	}
	strcpy(config.ota.hostname, doc["OTA"]["host"]);    // "esp8266-chipID"

	JsonObject MQTT = doc["MQTT"];
	strcpy(config.mqtt.broker, MQTT["broker"]);            // "brocker-hostname"
	config.mqtt.broker_port = MQTT["broker_port"];         // 1883
	strcpy(config.mqtt.client_ID, MQTT["client_ID"]);      // "esp8266chipID"
	config.mqtt.keepalive = MQTT["keepalive"];             // 15
	strcpy(config.mqtt.will_topic, MQTT["will_topic"]);    // "ESP/esp8266chipID/status"
	strcpy(config.mqtt.will_msg, MQTT["will_msg"]);        // "off"
	strcpy(config.mqtt.user, MQTT["user"]);                // "mqtt-user"
	strcpy(config.mqtt.pass, MQTT["pass"]);                // "mqtt-password"
	strcpy(config.http.user, doc["HTTP"]["user"]);         // "admin"
	strcpy(config.http.pass, doc["HTTP"]["pass"]);         // "admin"

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
	WiFi["ssid"] = config.wifi.ssid;
	WiFi["pass"] = config.wifi.pass;
	WiFi["mode"] = config.wifi.mode;
	if (basicSetup._staticIP) {
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


BasicWiFi::BasicWiFi(const char *ssid, const char *pass, int mode)
    : _ssid(ssid)
    , _pass(pass)
    , _mode(mode)
    , _staticIP(false) {
	strcpy(config.wifi.ssid, ssid);
	strcpy(config.wifi.pass, pass);
	config.wifi.mode = mode;
	basicSetup._inclWiFi = true;
	basicSetup._staticIP = false;
}
BasicWiFi::BasicWiFi(const char *ssid, const char *pass, int mode, const char *ip, const char *subnet, const char *gateway, const char *dns1, const char *dns2)
    : _ssid(ssid)
    , _pass(pass)
    , _mode(mode)
    , _staticIP(true) {
	_ip.fromString(ip);
	_subnet.fromString(subnet);
	_gateway.fromString(gateway);
	_dns1.fromString(dns1);
	_dns2.fromString(dns2);
	strcpy(config.wifi.ssid, ssid);
	strcpy(config.wifi.pass, pass);
	config.wifi.mode = mode;
	(config.wifi.IP).fromString(ip);
	(config.wifi.subnet).fromString(subnet);
	(config.wifi.gateway).fromString(gateway);
	(config.wifi.dns1).fromString(dns1);
	(config.wifi.dns2).fromString(dns2);
	basicSetup._inclWiFi = true;
	basicSetup._staticIP = true;
}
void BasicWiFi::setup(bool waitForConnection) {
	if (_staticIP) {
		WiFi.config(_ip, _gateway, _subnet, _dns1, _dns2);
	}
	WiFi.mode((WiFiMode_t)_mode);
	WiFi.persistent(false);
	WiFi.begin(_ssid, _pass);
	WiFiConnectedHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &evt) {
		Serial.println("WiFi connected!\n SSID: " + WiFi.SSID());
	});
	gotIpHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP &evt) {
		Serial.println(" IP:   " + WiFi.localIP().toString());
		if (basicSetup._inclOTA) {
			ArduinoOTA.begin();
			Serial.println("OTA started!");
		}
		if (basicSetup._inclWebEditor) {
			editorServer.begin();
			Serial.println("webEditor started!");
		}
		if (basicSetup._inclMQTT) {
			mqttReconnectTimer.once(1, []() {
				AclientMQTT.connect();
			});
		}
	});
	WiFiDisconnectedHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected &evt) {
		WiFi.disconnect(true);
		Serial.println("WiFi disconnected, reconnecting!");
		if (basicSetup._inclOTA) {
		}
		if (basicSetup._inclMQTT) {
			mqttReconnectTimer.detach();
		}
		if (basicSetup._inclWebEditor) {
		}
		wifiReconnectTimer.once(2, [&]() {
			WiFi.begin(_ssid, _pass);
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
	sprintf(config.ota.hostname, "esp8266-%06x", ESP.getChipId());
	strcpy(_hostname, config.ota.hostname);
	basicSetup._inclOTA = true;
};
BasicOTA::BasicOTA(const char *hostname) {
	strcpy(config.ota.hostname, hostname);
	strcpy(_hostname, hostname);
	basicSetup._inclOTA = true;
};
void BasicOTA::setup() {
	ArduinoOTA.setHostname(_hostname);
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


BasicMQTT::BasicMQTT(const char *broker_address, int broker_port, const char *clientID, int keepAlive, const char *willTopic, const char *willMsg, const char *user, const char *pass)
    : _broker_address(broker_address)
    , _broker_port(broker_port)
    , _clientID(clientID)
    , _keepAlive(keepAlive)
    , _willTopic(willTopic)
    , _willMsg(willMsg)
    , _user(user)
    , _pass(pass) {
	strcpy(config.mqtt.broker, broker_address);
	config.mqtt.broker_port = broker_port;
	// sprintf(config.mqtt.client_ID, "esp8266-%06x", ESP.getChipId());
	strcpy(config.mqtt.client_ID, clientID);
	config.mqtt.keepalive = keepAlive;
	// sprintf(will_topic, MQTT_WILL_TOPIC, client_ID);
	strcpy(config.mqtt.will_topic, willTopic);
	strcpy(config.mqtt.will_msg, willMsg);
	strcpy(config.mqtt.user, user);
	strcpy(config.mqtt.pass, pass);
	basicSetup._inclMQTT = true;
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
uint16_t subscribe(const char *topic, uint8_t qos) {
	return AclientMQTT.subscribe(topic, qos);
}
void BasicMQTT::_onConnect() {
	Serial.println((String) "MQTT connected!\n " + AclientMQTT.getClientId() + "@" + _broker_address);
	uint16_t subCommands = AclientMQTT.subscribe(((String) "ESP/" + AclientMQTT.getClientId() + "/commands").c_str(), 2);
	AclientMQTT.publish(((String) "ESP/" + AclientMQTT.getClientId() + "/status").c_str(), 2, true, (uint8_t *)"on", strlen("on"), false);
	for (const auto &handler : _onConnectHandler) handler();
}
void BasicMQTT::_onMessage(const char *_topic, const char *_payload) {
	for (const auto &handler : _onMessageHandler) handler(_topic, _payload);
}
void BasicMQTT::setup(bool waitForConnection) {
	AclientMQTT.setClientId(_clientID);
	AclientMQTT.setKeepAlive(_keepAlive);
#if MQTT_SET_LASTWILL
	AclientMQTT.setWill(_willTopic, 2, true, _willMsg);
#endif
#if MQTT_USE_CREDENTIALS
	AclientMQTT.setCredentials(_user, _pass);
#endif
	AclientMQTT.setServer(_broker_address, _broker_port);
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
			digitalWrite(LED_BUILTIN, LOW);
			Serial.print(".");
			delay(100);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(150);
			if (retry >= 40) {
				Serial.println("Can't connect to MQTT!");
				break;
			}
			retry++;
		}
	}
}


BasicFS::BasicFS() {
	Serial.println("mount 0");
	basicSetup._fsStarted = basicFS.setup();
}
bool BasicFS::setup() {
	while (!LittleFS.begin()) {
		Serial.println("LittleFS mount failed!");
		return false;
	}
	Serial.println("LittleFS mounted!");
	return true;
}

BasicFileEditor::BasicFileEditor()
    : _user("admin")
    , _pass("admin") {
	strcpy(config.http.user, _user);
	strcpy(config.http.pass, _pass);
	basicSetup._inclWebEditor = true;
}
BasicFileEditor::BasicFileEditor(const char *user, const char *pass)
    : _user(user)
    , _pass(pass) {
	strcpy(config.http.user, user);
	strcpy(config.http.pass, pass);
	basicSetup._inclWebEditor = true;
}
void BasicFileEditor::setup() {
	if (!(basicSetup._fsStarted)) {
		Serial.println("mount 2");
		basicSetup._fsStarted = basicFS.setup();
	}
	if (basicSetup._fsStarted) {
		editorServer.addHandler(new SPIFFSEditor(_user, _pass, LittleFS));
		editorServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->redirect("/edit");
		});
	}
}
