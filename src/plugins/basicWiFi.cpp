#include "basicWiFi.hpp"


WiFiEventHandler _WiFiConnectedHandler, _WiFiGotIpHandler, _WiFiDisconnectedHandler;
Ticker _wifiReconnectTimer;

char BasicWiFi::_ssid[32];
char BasicWiFi::_pass[64];
WiFiMode_t BasicWiFi::_mode;
bool BasicWiFi::_staticIP;
IPAddress BasicWiFi::_IP;         // optional
IPAddress BasicWiFi::_subnet;     // optional
IPAddress BasicWiFi::_gateway;    // optional
IPAddress BasicWiFi::_dns1;       // optional
IPAddress BasicWiFi::_dns2;       // optional

BasicWiFi::BasicWiFi(const char *ssid, const char *pass, int mode) {
	strcpy(_ssid, ssid);
	strcpy(_pass, pass);
	_mode = static_cast<WiFiMode_t>(mode);
    _staticIP = false;
}
BasicWiFi::BasicWiFi(const char *ssid, const char *pass, int mode, const char *IP, const char *subnet, const char *gateway, const char *dns1, const char *dns2) {
	strcpy(_ssid, ssid);
	strcpy(_pass, pass);
	_mode = static_cast<WiFiMode_t>(mode);
    _staticIP = true;
	(_IP).fromString(IP);
	(_subnet).fromString(subnet);
	(_gateway).fromString(gateway);
	(_dns1).fromString(dns1);
	(_dns2).fromString(dns2);
}
BasicWiFi::~BasicWiFi() {
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
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_debug, "WiFi connected to: " + WiFi.SSID() + " [" + WiFi.BSSIDstr() + "]");
	}
	for (const auto &handler : _onConnectHandler) handler(evt);
}
void BasicWiFi::_onGotIP(const WiFiEventStationModeGotIP &evt) {
	BASICWIFI_PRINTLN(" IP:   " + WiFi.localIP().toString());
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_debug, "got IP [" + (WiFi.localIP()).toString() + "]");
	}
	if (_basicSetup._inclOTA) {
		ArduinoOTA.begin();
		BASICOTA_PRINTLN("OTA started!");
	}
	if (_basicSetup._inclServerHttp) {
		_serverHttp.begin();
		BASICSERVERHTTP_PRINTLN("http server started!");
	}
	if (BasicSetup::_inclMQTT) {
		_mqttReconnectTimer.once(1, []() {
			_clientMQTT.connect();
		});
	}
	for (const auto &handler : _onGotIPhandler) handler(evt);
}
void BasicWiFi::_onDisconnected(const WiFiEventStationModeDisconnected &evt) {
	WiFi.disconnect(true);
	BASICWIFI_PRINTLN("WiFi disconnected, reconnecting!");
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_debug, "WiFi disconnected [" + String(_wifiStatus[WiFi.status()]) + "]");
	}
	if (_basicSetup._inclOTA) {
	}
	if (BasicSetup::_inclMQTT) {
		_mqttReconnectTimer.detach();
	}
	if (_basicSetup._inclServerHttp) {
	}
	_wifiReconnectTimer.once(5, [&]() {
		WiFi.begin(_ssid, _pass);
	});
	for (const auto &handler : _onDisconnectHandler) handler(evt);
}
void BasicWiFi::setup(bool staticIP) {
	if (staticIP) {
		WiFi.config(_IP, _gateway, _subnet, _dns1, _dns2);
	}
	WiFi.mode(_mode);
	WiFi.persistent(false);
	WiFi.begin(_ssid, _pass);
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
