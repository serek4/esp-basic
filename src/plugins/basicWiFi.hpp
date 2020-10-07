#pragma once

#include "../espBasicSetup.hpp"
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <functional>
#include <vector>


namespace WiFiUserHandlers {
typedef std::function<void(const WiFiEventStationModeConnected &evt)> onWiFiConnectHandler;
typedef std::function<void(const WiFiEventStationModeGotIP &evt)> onWiFiGotIPhandler;
typedef std::function<void(const WiFiEventStationModeDisconnected &evt)> onWiFiDisconnectHandler;
}    // namespace WiFiUserHandlers

class BasicWiFi {
  public:
	void setup(bool staticIP);
	void waitForWiFi(int waitTime = 10);
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
	bool _inclWiFi;

	friend class BasicSetup;
};

extern BasicWiFi _basicWiFi;
