#include "basicWiFi.hpp"


WiFiEventHandler _WiFiConnectedHandler, _WiFiGotIpHandler, _WiFiDisconnectedHandler;
Ticker _wifiReconnectTimer;

BasicWiFi::BasicWiFi()
    : _inclWiFi(true) {
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
	BASICWIFI_PRINTLN("WiFi connected!\n SSID: " + WiFi.SSID());
	for (const auto &handler : _onConnectHandler) handler(evt);
}
void BasicWiFi::_onGotIP(const WiFiEventStationModeGotIP &evt) {
	BASICWIFI_PRINTLN(" IP:   " + WiFi.localIP().toString());
	if (_basicSetup._inclOTA) {
		ArduinoOTA.begin();
		BASICOTA_PRINTLN("OTA started!");
	}
	if (_basicSetup._inclServerHttp) {
		_serverHttp.begin();
		BASICSERVERHTTP_PRINTLN("http server started!");
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
	BASICWIFI_PRINTLN("WiFi disconnected, reconnecting!");
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
void BasicWiFi::setup(bool staticIP) {
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
}
void BasicWiFi::waitForWiFi(int waitTime) {
	BASICWIFI_PRINT("Waiting for WiFi connection");
	u_long startWaitingAt = millis();
	while (WiFi.status() != WL_CONNECTED) {
		BASICWIFI_PRINT(".");
		if (BasicSetup::_useLed) {
			digitalWrite(LED_BUILTIN, LOW);
			delay(200);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(300);
		} else {
			delay(500);
		}
		if (millis() - startWaitingAt > waitTime * 1000) {
			BASICWIFI_PRINTLN("Can't connect to WiFi!");
			return;
		}
	}
	_checkConnection();
}
void BasicWiFi::_checkConnection() {
	IPAddress buffer;
	BASICWIFI_PRINT("checking DNS server");
	int retry = 0;
	while (WiFi.hostByName("google.com", buffer) == 0) {
		BASICWIFI_PRINT(".");
		delay(100);
		retry++;
		if (retry > 3) {
			BASICWIFI_PRINTLN("DNS does not work!");
			return;
		}
	}
	BASICWIFI_PRINTLN(" OK!");
}

BasicWiFi _basicWiFi;
