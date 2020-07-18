#include "espBasicSetup.hpp"

basicSetup::Config config;
WiFiEventHandler WiFiConnectedHandler, gotIpHandler, WiFiDisconnectedHandler;
Ticker wifiReconnectTimer;
AsyncMqttClient AclientMQTT;
Ticker mqttReconnectTimer;
AsyncWebServer editorServer(80);

basicSetup::basicSetup()
    : _fsStarted(false)
    , _inclConfigFile(true)
    , _inclOTA(true)
    , _inclMQTT(true)
    , _inclWebEditor(true) {}
basicSetup::basicSetup(bool inclConfigFile, bool inclOTA, bool inclMQTT, bool inclWebEditor)
    : _fsStarted(false) {
	_inclConfigFile = inclConfigFile;
	_inclOTA = inclOTA;
	_inclMQTT = inclMQTT;
	_inclWebEditor = inclWebEditor;
}
void basicSetup::begin(bool waitForWiFi, bool waitForMQTT) {
	Serial.begin(115200);
	Serial.println("");
	if (_inclConfigFile) {
		_fsStarted = FSsetup();
		if (_fsStarted) {
			if (!config.loadConfig()) {
				if (!config.loadConfig("backup-config.json")) {
					Serial.println("Loading default settings!");
					config.createConfig();
				}
			}
		}
	}
	if (_inclOTA) {
		OTAsetup();
	}
	if (_inclWebEditor && _fsStarted) {
		HTTPsetup();
	}
	WiFiSetup(waitForWiFi);
	if (_inclMQTT) {
		MQTTsetup(waitForMQTT);
	}
}

basicSetup::Config::WiFi::WiFi() {
	strcpy(ssid, WIFI_SSID);
	strcpy(pass, WIFI_PASS);
	mode = WIFI_MODE;
#if STATIC_IP
	IP.fromString(WIFI_IP);
	subnet.fromString(WIFI_SUBNET);
	gateway.fromString(WIFI_GATEWAY);
	DNS1.fromString(WIFI_DNS1);
	DNS2.fromString(WIFI_DNS2);
#endif
};
basicSetup::Config::OTA::OTA() {
#ifndef OTA_HOST
	sprintf(hostname, "esp8266-%06x", ESP.getChipId());
#else
	strcpy(hostname, OTA_HOST);
#endif
};
basicSetup::Config::MQTT::MQTT() {
	strcpy(broker, MQTT_BROKER);
	broker_port = MQTT_BROKER_PORT;
#ifndef MQTT_CLIENTID
	sprintf(client_ID, "esp8266-%06x", ESP.getChipId());
#else
	strcpy(client_ID, MQTT_CLIENTID);
#endif
	keepalive = MQTT_KEEPALIVE;
#if MQTT_SET_LASTWILL
	sprintf(will_topic, MQTT_WILL_TOPIC, client_ID);
	strcpy(will_msg, MQTT_WILL_MSG);
#endif
#if MQTT_USE_CREDENTIALS
	strcpy(user, MQTT_USER);
	strcpy(pass, MQTT_PASS);
#endif
};
basicSetup::Config::HTTP::HTTP() {
	strcpy(user, HTTP_USER);
	strcpy(pass, HTTP_PASS);
};
bool basicSetup::Config::loadConfig(String filename) {
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
#if STATIC_IP
	(config.wifi.IP).fromString((const char *)WiFi["IP"]);              // "192.168.0.150"
	(config.wifi.subnet).fromString((const char *)WiFi["subnet"]);      // "255.255.255.0"
	(config.wifi.gateway).fromString((const char *)WiFi["gateway"]);    // "192.168.0.1"
	(config.wifi.DNS1).fromString((const char *)WiFi["DNS1"]);          // "192.168.0.1"
	(config.wifi.DNS2).fromString((const char *)WiFi["DNS2"]);          // "1.1.1.1"
#endif
	strcpy(config.ota.hostname, doc["OTA"]["host"]);    // "esp8266-chipID"

	JsonObject MQTT = doc["MQTT"];
	strcpy(config.mqtt.broker, MQTT["broker"]);          // "brocker-hostname"
	config.mqtt.broker_port = MQTT["broker_port"];       // 1883
	strcpy(config.mqtt.client_ID, MQTT["client_ID"]);    // "esp8266chipID"
	config.mqtt.keepalive = MQTT["keepalive"];           // 15
#if MQTT_SET_LASTWILL
	strcpy(config.mqtt.will_topic, MQTT["will_topic"]);    // "ESP/esp8266chipID/status"
	strcpy(config.mqtt.will_msg, MQTT["will_msg"]);        // "off"
#endif
#if MQTT_USE_CREDENTIALS
	strcpy(config.mqtt.user, MQTT["user"]);    // "mqtt-user"
	strcpy(config.mqtt.pass, MQTT["pass"]);    // "mqtt-password"
#endif
	strcpy(config.http.user, doc["HTTP"]["user"]);    // "admin"
	strcpy(config.http.pass, doc["HTTP"]["pass"]);    // "admin"

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
size_t basicSetup::Config::createConfig(String filename, bool save) {
	const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(8) + 390;
	DynamicJsonDocument doc(capacity);

	JsonObject WiFi = doc.createNestedObject("WiFi");
	WiFi["ssid"] = config.wifi.ssid;
	WiFi["pass"] = config.wifi.pass;
	WiFi["mode"] = config.wifi.mode;
#if STATIC_IP
	WiFi["IP"] = (config.wifi.IP).toString();
	WiFi["subnet"] = (config.wifi.subnet).toString();
	WiFi["gateway"] = (config.wifi.gateway).toString();
	WiFi["DNS1"] = (config.wifi.DNS1).toString();
	WiFi["DNS2"] = (config.wifi.DNS2).toString();
#endif
	JsonObject OTA = doc.createNestedObject("OTA");
	OTA["host"] = config.ota.hostname;

	JsonObject MQTT = doc.createNestedObject("MQTT");
	MQTT["broker"] = config.mqtt.broker;
	MQTT["broker_port"] = config.mqtt.broker_port;
	MQTT["client_ID"] = config.mqtt.client_ID;
	MQTT["keepalive"] = config.mqtt.keepalive;
#if MQTT_SET_LASTWILL
	MQTT["will_topic"] = config.mqtt.will_topic;
	MQTT["will_msg"] = config.mqtt.will_msg;
#endif
#if MQTT_USE_CREDENTIALS
	MQTT["user"] = config.mqtt.user;
	MQTT["pass"] = config.mqtt.pass;
#endif
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

void basicSetup::WiFiSetup(bool &waitForConnection) {
#if STATIC_IP
	WiFi.config(config.wifi.IP, config.wifi.gateway, config.wifi.subnet, config.wifi.DNS1, config.wifi.DNS2);
#endif
	WiFi.mode(config.wifi.mode);
	WiFi.persistent(false);
	WiFi.begin(config.wifi.ssid, config.wifi.pass);
	WiFiConnectedHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &evt) {
		Serial.println("WiFi connected!\n SSID: " + WiFi.SSID());
	});
	gotIpHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP &evt) {
		Serial.println(" IP:   " + WiFi.localIP().toString());
		if (_inclOTA) {
			ArduinoOTA.begin();
			Serial.println("OTA started!");
		}
		if (_inclWebEditor) {
			editorServer.begin();
			Serial.println("webEditor started!");
		}
		if (_inclMQTT) {
			mqttReconnectTimer.once(1, []() {
				AclientMQTT.connect();
			});
		}
	});
	WiFiDisconnectedHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected &evt) {
		WiFi.disconnect(true);
		Serial.println("WiFi disconnected, reconnecting!");
		if (_inclOTA) {
		}
		if (_inclMQTT) {
			mqttReconnectTimer.detach();
		}
		if (_inclWebEditor) {
		}
		wifiReconnectTimer.once(2, []() {
			WiFi.begin(config.wifi.ssid, config.wifi.pass);
		});
	});
	if (waitForConnection) {
		waitForWiFi();
	}
}
void basicSetup::waitForWiFi() {
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

void basicSetup::OTAsetup() {
	ArduinoOTA.setHostname(config.ota.hostname);
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

void basicSetup::onMQTTconnect(const UserHandlers::onMQTTconnectHandler &handler) {
	_onConnectHandler.push_back(handler);
}
void basicSetup::onMQTTmessage(const UserHandlers::onMQTTmesageHandler &handler) {
	_onMessageHandler.push_back(handler);
}
uint16_t basicSetup::MQTTpublish(const char *topic, const char *payload, uint8_t qos, bool retain) {
	return AclientMQTT.publish(topic, qos, retain, payload);
}
uint16_t MQTTsubscribe(const char *topic, uint8_t qos) {
	return AclientMQTT.subscribe(topic, qos);
}
void basicSetup::_onMQTTconnect() {
	Serial.println((String) "MQTT connected!\n " + AclientMQTT.getClientId() + "@" + config.mqtt.broker);
	uint16_t subCommands = AclientMQTT.subscribe(((String) "ESP/" + AclientMQTT.getClientId() + "/commands").c_str(), 2);
	uint16_t pubStatus = AclientMQTT.publish(((String) "ESP/" + AclientMQTT.getClientId() + "/status").c_str(), 2, true, "on");
	for (const auto &handler : _onConnectHandler) handler();
}
void basicSetup::_onMQTTmessage(char *_topic, char *_payload) {
	for (const auto &handler : _onMessageHandler) handler(_topic, _payload);
}
void basicSetup::MQTTsetup(bool &waitForConnection) {
	AclientMQTT.setClientId(config.mqtt.client_ID);
	AclientMQTT.setKeepAlive(config.mqtt.keepalive);
#if MQTT_SET_LASTWILL
	AclientMQTT.setWill(config.mqtt.will_topic, 2, true, config.mqtt.will_msg);
#endif
#if MQTT_USE_CREDENTIALS
	AclientMQTT.setCredentials(config.mqtt.user, config.mqtt.pass);
#endif
	AclientMQTT.setServer(config.mqtt.broker, config.mqtt.broker_port);
	AclientMQTT.onConnect([&](bool sessionPresent) {
		_onMQTTconnect();
	});
	AclientMQTT.onMessage([&](char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
		char fixedPayload[len + 1];
		fixedPayload[len] = '\0';
		_onMQTTmessage(topic, strncpy(fixedPayload, payload, len));
	});
	AclientMQTT.onDisconnect([](AsyncMqttClientDisconnectReason reason) {
		Serial.println((String) "MQTT disconnected: [" + (int)reason + "]!");
		if (WiFi.isConnected()) {
			mqttReconnectTimer.once(10, []() { AclientMQTT.connect(); });
		}
	});
	if (waitForConnection) {
		waitForMQTT();
	}
}
void basicSetup::waitForMQTT() {
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

bool basicSetup::FSsetup() {
	while (!LittleFS.begin()) {
		Serial.println("LittleFS mount failed!");
		return false;
	}
	Serial.println("LittleFS mounted!");
	return true;
}
void basicSetup::HTTPsetup() {
	editorServer.addHandler(new SPIFFSEditor(config.http.user, config.http.pass, LittleFS));
	editorServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->redirect("/edit");
	});
}
