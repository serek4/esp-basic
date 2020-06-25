#include "espBasicSetup.hpp"
#include "secrets.h"

WiFiEventHandler WiFiConnectedHandler, gotIpHandler, WiFiDisconnectedHandler;
Ticker wifiReconnectTimer;
AsyncMqttClient AclientMQTT;
Ticker mqttReconnectTimer;
AsyncWebServer editorServer(80);

bool inclOTA;
bool inclMQTT;
bool inclWebEditor;

basicSetup::basicSetup()
    : _fsStarted(false) {
	inclOTA = true;
	inclMQTT = true;
	inclWebEditor = true;
}
basicSetup::basicSetup(bool _inclOTA, bool _inclMQTT, bool _inclWebEditor)
    : _fsStarted(false) {
	inclOTA = _inclOTA;
	inclMQTT = _inclMQTT;
	inclWebEditor = _inclWebEditor;
}
void basicSetup::begin(bool waitForWiFi, bool waitForMQTT) {
	Serial.begin(115200);
	Serial.println("");
	WiFiSetup(waitForWiFi);
	if (inclOTA) {
		OTAsetup();
	}
	if (inclMQTT) {
		MQTTsetup(waitForMQTT);
	}
	if (inclWebEditor) {
		if (FSsetup()) {
			HTTPsetup();
		}
	}
}
void basicSetup::WiFiSetup(bool &waitForConnection) {
#if defined WIFI_IP && defined WIFI_SUBNET && defined WIFI_GATEWAY
	IPAddress ip;
	ip.fromString(WIFI_IP);
	IPAddress subnet;
	subnet.fromString(WIFI_SUBNET);
	IPAddress gateway;
	gateway.fromString(WIFI_GATEWAY);
	IPAddress dns1;
	dns1.fromString(WIFI_DNS1);
#ifndef WIFI_DNS2
	WiFi.config(ip, gateway, subnet, dns1);
#else
	IPAddress dns2;
	dns2.fromString(WIFI_DNS2);
	WiFi.config(ip, gateway, subnet, dns1, dns2);
#endif
#endif
	WiFi.mode(WIFI_MODE);
	WiFi.persistent(false);
	WiFi.begin(WIFI_SSID, WIFI_PASS);
	WiFiConnectedHandler = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &evt) {
		Serial.println("WiFi connected!\n SSID: " + WiFi.SSID());
	});
	gotIpHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &evt) {
		Serial.println(" IP:   " + WiFi.localIP().toString());
		if (inclOTA) {
			ArduinoOTA.begin();
			Serial.println("OTA started!");
		}
		if (inclMQTT) {
			mqttReconnectTimer.once(1, []() {
				AclientMQTT.connect();
			});
		}
		if (inclWebEditor) {
		}
	});
	WiFiDisconnectedHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &evt) {
		WiFi.disconnect(true);
		Serial.println("WiFi disconnected, reconnecting!");
		if (inclOTA) {
		}
		if (inclMQTT) {
			mqttReconnectTimer.detach();
		}
		if (inclWebEditor) {
		}
		wifiReconnectTimer.once(2, []() {
			WiFi.begin(WIFI_SSID, WIFI_PASS);
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
#ifdef OTA_HOST
	ArduinoOTA.setHostname(OTA_HOST);
#endif
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

void basicSetup::MQTTsetup(bool &waitForConnection) {
#ifdef MQTT_CLIENTID
	AclientMQTT.setClientId(MQTT_CLIENTID);
#endif
#ifdef MQTT_KEEPALIVE
	AclientMQTT.setKeepAlive(MQTT_KEEPALIVE);
#endif
#if defined MQTT_WILL_TOPIC && defined MQTT_WILL_MSG
	AclientMQTT.setWill(MQTT_WILL_TOPIC, 2, true, MQTT_WILL_MSG);
#endif
#if defined MQTT_USER && defined MQTT_PASS
	AclientMQTT.setCredentials(MQTT_USER, MQTT_PASS);
#endif
	AclientMQTT.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
	AclientMQTT.onConnect([](bool sessionPresent) {
		Serial.println((String) "MQTT connected!\n " + AclientMQTT.getClientId() + "@" + MQTT_BROKER);
		uint16_t subCommands = AclientMQTT.subscribe(((String) "ESP/" + AclientMQTT.getClientId() + "/commands").c_str(), 2);
		uint16_t pubStatus = AclientMQTT.publish(((String) "ESP/" + AclientMQTT.getClientId() + "/status").c_str(), 2, true, "on");
	});
	AclientMQTT.onMessage([](char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
		char fixedPayload[len + 1];
		fixedPayload[len] = '\0';
		MQTTmessage(topic, strncpy(fixedPayload, payload, len));
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
	editorServer.addHandler(new SPIFFSEditor(HTTP_USER, HTTP_PASS, LittleFS));
	editorServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->redirect("/edit");
	});
	editorServer.begin();
	Serial.println("webEditor started!");
}
