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

class BasicFS {
  public:
	bool setup();

	BasicFS();
};

class ImportSetup {
  public:
	void WIFIsettings(const char *ssid, const char *pass, int mode, bool staticIP, const char *IP, const char *subnet, const char *gateway, const char *dns1, const char *dns2);
	void OTAsettings(const char *hostname);
	void MQTTsettings(const char *broker_address, int broker_port, const char *clientID, int keepAlive, const char *willTopic, const char *willMsg, const char *user, const char *pass);
	void ServerHttpSettings(const char *user, const char *pass);

  private:
};

class BasicConfig {
  public:
	void setup();
	size_t createConfig(ConfigData &config, String filename = "config.json", bool save = true);
	bool loadConfig(ConfigData &config, String filename = "config.json");

	BasicConfig();

  private:
	bool _checkJsonVariant(char *saveTo, JsonVariant string);
	bool _checkJsonVariant(IPAddress &saveTo, JsonVariant IPstring);
	bool _checkJsonVariant(int &saveTo, JsonVariant number);
};


class BasicServerHttp {
  public:
	void setup();

	BasicServerHttp();
};


namespace MQTTuserHandlers {
typedef std::function<void()> onMQTTconnectHandler;
typedef std::function<void(const char *_topic, const char *_payload)> onMQTTmesageHandler;
typedef std::function<void(int8_t reason)> onMQTTdisconnectHandler;
}    // namespace MQTTuserHandlers

class BasicMQTT {
  public:
	void setup(bool waitForConnection);
	void waitForMQTT();
	void onConnect(const MQTTuserHandlers::onMQTTconnectHandler &handler);
	void onMessage(const MQTTuserHandlers::onMQTTmesageHandler &handler);
	void onDisconnect(const MQTTuserHandlers::onMQTTdisconnectHandler &handler);
	void publish(const char *topic, const char *payload, uint8_t qos = 0, bool retain = false);
	void publish(const char *topic, int payload, uint8_t qos = 0, bool retain = false);
	void publish(const char *topic, float payload, uint8_t qos = 0, bool retain = false) { publish(topic, payload, 3, 2, qos, retain); };
	void publish(const char *topic, float payload, signed char width, unsigned char prec, uint8_t qos = 0, bool retain = false);
	uint16_t subscribe(const char *topic, uint8_t qos = 0);

	BasicMQTT();

  private:
	std::vector<MQTTuserHandlers::onMQTTconnectHandler> _onConnectHandler;
	std::vector<MQTTuserHandlers::onMQTTmesageHandler> _onMessageHandler;
	std::vector<MQTTuserHandlers::onMQTTdisconnectHandler> _onDisconnectHandler;
	void _onConnect();
	void _onMessage(const char *_topic, const char *_payload);
	void _onDisconnect(int8_t reason);
};


class BasicOTA {
  public:
	void setup();

	BasicOTA();
};


namespace WiFiUserHandlers {
typedef std::function<void(const WiFiEventStationModeConnected &evt)> onWiFiConnectHandler;
typedef std::function<void(const WiFiEventStationModeGotIP &evt)> onWiFiGotIPhandler;
typedef std::function<void(const WiFiEventStationModeDisconnected &evt)> onWiFiDisconnectHandler;
}    // namespace WiFiUserHandlers

class BasicWiFi {
  public:
	void setup(bool waitForConnection, bool staticIP);
	void waitForWiFi();
	void onConnected(const WiFiUserHandlers::onWiFiConnectHandler &handler);
	void onGotIP(const WiFiUserHandlers::onWiFiGotIPhandler &handler);
	void onDisconnected(const WiFiUserHandlers::onWiFiDisconnectHandler &handler);

	BasicWiFi();

  private:
	std::vector<WiFiUserHandlers::onWiFiConnectHandler> _onConnectHandler;
	std::vector<WiFiUserHandlers::onWiFiGotIPhandler> _onGotIPhandler;
	std::vector<WiFiUserHandlers::onWiFiDisconnectHandler> _onDisconnectHandler;
	void _checkConnection();
	void _onConnected(const WiFiEventStationModeConnected &evt);
	void _onGotIP(const WiFiEventStationModeGotIP &evt);
	void _onDisconnected(const WiFiEventStationModeDisconnected &evt);

	friend class BasicSetup;
};

class BasicSetup {
  public:
	void begin();
	ConfigData &config;
	BasicWiFi &WIFI;
	BasicMQTT &MQTT;
	AsyncWebServer &serverHttp;

	BasicSetup();

  private:
	bool _fsStarted;
	bool _inclServerHttp;
	bool _inclMQTT;
	bool _inclWiFi;
	bool _inclConfig;
	bool _inclOTA;
	bool _staticIP;

	friend class ImportSetup;
	friend class BasicFS;
	friend class BasicConfig;
	friend class BasicServerHttp;
	friend class BasicMQTT;
	friend class BasicOTA;
	friend class BasicWiFi;
};
