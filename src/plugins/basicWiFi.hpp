#pragma once

#include "../espBasicSetup.hpp"
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Ticker.h>
#include <functional>
#include <vector>


#define DEFAULT_RECONNECT_DELAY 10

typedef enum {
	wifi_connected,
	dns_fail,
	wifi_fail,
} WiFiStatus;

namespace WiFiUserHandlers {
#ifdef ARDUINO_ARCH_ESP32
typedef std::function<void(WiFiEvent_t event, WiFiEventInfo_t info)> onWiFiConnectHandler;
typedef std::function<void(WiFiEvent_t event, WiFiEventInfo_t info)> onWiFiGotIPhandler;
typedef std::function<void(WiFiEvent_t event, WiFiEventInfo_t info)> onWiFiDisconnectHandler;
#elif defined(ARDUINO_ARCH_ESP8266)
typedef std::function<void(const WiFiEventStationModeConnected &evt)> onWiFiConnectHandler;
typedef std::function<void(const WiFiEventStationModeGotIP &evt)> onWiFiGotIPhandler;
typedef std::function<void(const WiFiEventStationModeDisconnected &evt)> onWiFiDisconnectHandler;
#endif
}    // namespace WiFiUserHandlers

class BasicWiFi {
  public:
	void setup(bool staticIP);
	uint8_t waitForWiFi(int waitTime = 10);
	void onConnected(const WiFiUserHandlers::onWiFiConnectHandler &handler);
	void onGotIP(const WiFiUserHandlers::onWiFiGotIPhandler &handler);
	void onDisconnected(const WiFiUserHandlers::onWiFiDisconnectHandler &handler);
	static void connect();
	static void disconnect();
	static void reconnect(uint8_t reconnectDelay = DEFAULT_RECONNECT_DELAY);

	BasicWiFi(const char *ssid, const char *pass, int mode);
	BasicWiFi(const char *ssid, const char *pass, int mode, const char *IP, const char *subnet, const char *gateway, const char *dns1, const char *dns2);
	~BasicWiFi();

  private:
	static char _ssid[32];
	static char _pass[64];
	static WiFiMode_t _mode;
	static bool _staticIP;
	static IPAddress _IP;         // optional
	static IPAddress _subnet;     // optional
	static IPAddress _gateway;    // optional
	static IPAddress _dns1;       // optional
	static IPAddress _dns2;       // optional
#ifdef ARDUINO_ARCH_ESP32
	const char *_wifiStatus[7] = {"IDLE_STATUS", "NO_SSID_AVAIL", "SCAN_COMPLETED", "CONNECTED", "CONNECT_FAILED", "CONNECTION_LOST", "DISCONNECTED"};
#elif defined(ARDUINO_ARCH_ESP8266)
	const char *_wifiStatus[8] = {"IDLE_STATUS", "NO_SSID_AVAIL", "SCAN_COMPLETED", "CONNECTED", "CONNECT_FAILED", "CONNECTION_LOST", "WRONG_PASSWORD", "DISCONNECTED"};
#endif
	std::vector<WiFiUserHandlers::onWiFiConnectHandler> _onConnectHandler;
	std::vector<WiFiUserHandlers::onWiFiGotIPhandler> _onGotIPhandler;
	std::vector<WiFiUserHandlers::onWiFiDisconnectHandler> _onDisconnectHandler;
	uint8_t _checkConnection();
#ifdef ARDUINO_ARCH_ESP32
	void _onConnected(WiFiEvent_t event, WiFiEventInfo_t info);
	void _onGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
	void _onDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
#elif defined(ARDUINO_ARCH_ESP8266)
	void _onConnected(const WiFiEventStationModeConnected &evt);
	void _onGotIP(const WiFiEventStationModeGotIP &evt);
	void _onDisconnected(const WiFiEventStationModeDisconnected &evt);
#endif

	friend class BasicConfig;
	friend class EspBasicSetup;
};
