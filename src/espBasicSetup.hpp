#pragma once

#include "Arduino.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <PangolinMQTT.h>
#include <SPIFFSEditor.h>
#include <Ticker.h>
#include <functional>
#include <vector>

class espBasicSetup {
  public:
	void begin();

	espBasicSetup(){};
};

class SharedSetup {
  public:
	SharedSetup();

	bool _fsStarted;
	bool _inclWebEditor;
	bool _inclMQTT;
	bool _inclWiFi;
	bool _inclConfig;
	bool _inclOTA;
	bool _staticIP;
};


class BasicFS {
  public:
	bool setup();

	BasicFS();
};

class BasicConfig {
  public:
	// WiFi settings
	class WiFi {
	  public:
		char ssid[32];
		char pass[64];
		int mode;
		IPAddress IP;         // optional
		IPAddress subnet;     // optional
		IPAddress gateway;    // optional
		IPAddress dns1;       // optional
		IPAddress dns2;       // optional
		WiFi(){};
	};
	// OTA settings
	class OTA {
	  public:
		char hostname[32];
		OTA(){};
	};
	// MQTT settings
	class MQTT {
	  public:
		char broker[32];
		int broker_port;
		char client_ID[32];
		int keepalive;
		char will_topic[64];    // optional
		char will_msg[16];      // optional
		char user[16];          // optional
		char pass[16];          // optional
		MQTT(){};
	};
	// web file editor
	class HTTP {
	  public:
		char user[16];
		char pass[16];
		HTTP(){};
	};
	WiFi wifi;
	OTA ota;
	MQTT mqtt;
	HTTP http;
	void setup();
	size_t createConfig(String filename = "config.json", bool save = true);
	bool loadConfig(String filename = "config.json");

	BasicConfig();
};


class BasicFileEditor {
  public:
	void setup();

	BasicFileEditor();
	BasicFileEditor(const char *user, const char *pass);
};


namespace MQTTuserHandlers {
typedef std::function<void()> onMQTTconnectHandler;
typedef std::function<void(const char *_topic, const char *_payload)> onMQTTmesageHandler;
}    // namespace MQTTuserHandlers

class BasicMQTT {
  public:
	void setup(bool waitForConnection);
	void waitForMQTT();
	void onConnect(const MQTTuserHandlers::onMQTTconnectHandler &handler);
	void onMessage(const MQTTuserHandlers::onMQTTmesageHandler &handler);
	void publish(const char *topic, const char *payload, uint8_t qos = 0, bool retain = false);
	uint16_t subscribe(const char *topic, uint8_t qos = 0);

	BasicMQTT(const char *broker_address, int broker_port, const char *clientID, int keepAlive, const char *willTopic, const char *willMsg, const char *user, const char *pass);

  private:
	std::vector<MQTTuserHandlers::onMQTTconnectHandler> _onConnectHandler;
	std::vector<MQTTuserHandlers::onMQTTmesageHandler> _onMessageHandler;
	void _onConnect();
	void _onMessage(const char *_topic, const char *_payload);
};


class BasicOTA {
  public:
	void setup();

	BasicOTA();
	BasicOTA(const char *hostname);
};


class BasicWiFi {
  public:
	void waitForWiFi();
	void setup(bool waitForConnection);

	BasicWiFi(const char *ssid, const char *pass, int mode);
	BasicWiFi(const char *ssid, const char *pass, int mode, const char *ip, const char *subnet, const char *gateway, const char *dns1, const char *dns2);
};
