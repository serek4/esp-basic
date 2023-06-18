#include "basicMQTT.hpp"


PangolinMQTT _clientMQTT;
Ticker _mqttReconnectTimer;

char BasicMQTT::_broker_address[32];
int BasicMQTT::_broker_port;
char BasicMQTT::_client_ID[32];
int BasicMQTT::_keepalive;
char BasicMQTT::_will_topic[64];    // optional
char BasicMQTT::_will_msg[16];      // optional
char BasicMQTT::_user[16];          // optional
char BasicMQTT::_pass[16];          // optional

bool BasicMQTT::_connected = false;

BasicMQTT::BasicMQTT(const char *broker_address) {
	strcpy(_broker_address, broker_address);
	_broker_port = 1883;

#ifdef ARDUINO_ARCH_ESP32
	sprintf(_client_ID, "esp32-%12llX", ESP.getEfuseMac());
#elif defined(ARDUINO_ARCH_ESP8266)
	sprintf(_client_ID, "esp8266-%06x", ESP.getChipId());
#endif
	_keepalive = 15;
	//TODO default will and credentials
}
BasicMQTT::BasicMQTT(const char *broker_address, int broker_port, const char *clientID, int keepAlive, const char *willTopic, const char *willMsg, const char *user, const char *pass) {
	strcpy(_broker_address, broker_address);
	_broker_port = broker_port;
	strcpy(_client_ID, clientID);
	_keepalive = keepAlive;
	strcpy(_will_topic, willTopic);
	strcpy(_will_msg, willMsg);
	strcpy(_user, user);
	strcpy(_pass, pass);
}
BasicMQTT::~BasicMQTT() {
}

void BasicMQTT::connect() {
	BasicMQTT::_connected = true;
	_clientMQTT.connect();
}
void BasicMQTT::reconnect() {
	disconnect();
	_mqttReconnectTimer.once(5, []() {
		connect();
	});
}
void BasicMQTT::disconnect() {
	if (_connected) {
		BasicMQTT::_connected = false;
		_clientMQTT.disconnect();
		_mqttReconnectTimer.detach();
	}
}

void BasicMQTT::onConnect(const MQTTuserHandlers::onMQTTconnectHandler &handler) {
	_onConnectHandler.push_back(handler);
}
void BasicMQTT::onMessage(const MQTTuserHandlers::onMQTTmesageHandler &handler) {
	_onMessageHandler.push_back(handler);
}
void BasicMQTT::onDisconnect(const MQTTuserHandlers::onMQTTdisconnectHandler &handler) {
	_onDisconnectHandler.push_back(handler);
}
void BasicMQTT::publish(const char *topic, const char *payload, uint8_t qos, bool retain) {
	_clientMQTT.publish(topic, payload, strlen(payload), qos, retain);
}
void BasicMQTT::publish(const char *topic, int payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	itoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
}
void BasicMQTT::publish(const char *topic, uint16_t payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	utoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
}
void BasicMQTT::publish(const char *topic, long payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	ltoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
}
void BasicMQTT::publish(const char *topic, u_long payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	ultoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
}
void BasicMQTT::publish(const char *topic, float payload, signed char width, unsigned char prec, uint8_t qos, bool retain) {
	char numberBuffer[12];
	dtostrf(payload, width, prec, numberBuffer);
	_clientMQTT.publish(topic, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
}
void BasicMQTT::subscribe(const char *topic, uint8_t qos) {
	return _clientMQTT.subscribe(topic, qos);
}
bool BasicMQTT::connected() {
	return _connected;
}

void BasicMQTT::_onConnect() {
	BASICMQTT_PRINTLN((String) "MQTT connected!\n " + _clientMQTT.getClientId() + "@" + _broker_address);
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_debug, (String) "MQTT connected [" + _clientMQTT.getClientId() + "@" + _broker_address + "]");
	}
	_mqttReconnectTimer.detach();
	_clientMQTT.publish(((String) "esp/" + _clientMQTT.getClientId() + "/status").c_str(), STATUS_ON_MSG, strlen(STATUS_ON_MSG), 0, true);
	_clientMQTT.subscribe(((String) "esp/" + _clientMQTT.getClientId() + "/commands").c_str(), 0);
	_clientMQTT.subscribe(((String) "esp/" + _clientMQTT.getClientId() + "/status").c_str(), 0);
	for (const auto &handler : _onConnectHandler) handler();
}
void BasicMQTT::_onMessage(const char *_topic, const char *_payload) {
	if (strcmp(_topic, _will_topic) == 0) {
		if (strcmp(_payload, _will_msg) == 0) {
			_clientMQTT.publish(((String) "esp/" + _clientMQTT.getClientId() + "/status").c_str(), STATUS_ON_MSG, strlen(STATUS_ON_MSG), 0, true);
		}
	}
	for (const auto &handler : _onMessageHandler) handler(_topic, _payload);
}
void BasicMQTT::_onDisconnect(int8_t reason) {
	BASICMQTT_PRINTLN("MQTT disconnected: [" + String(reason, 10) + "]!");
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(
		    now(),
		    ll_debug,
		    "MQTT disconnected [" + String(_MQTTerror[(reason < 0) ? 12 : reason]) + (reason < 0 ? "(" + String(reason, 10) + ")]" : "]"));
	}
	if (_connected) {
		_mqttReconnectTimer.attach(_keepalive * PANGO_POLL_RATE, []() { connect(); });
	}
	for (const auto &handler : _onDisconnectHandler) handler(reason);
}
void BasicMQTT::setup() {
	_clientMQTT.setClientId(_client_ID);
	_clientMQTT.setKeepAlive(_keepalive);
	_clientMQTT.setWill(_will_topic, 2, true, _will_msg);
	_clientMQTT.setCredentials(_user, _pass);
	_clientMQTT.setServer(_broker_address, _broker_port);
	_clientMQTT.onConnect([&](bool sessionPresent) {
		_onConnect();
	});
	_clientMQTT.onMessage([&](const char *topic, const uint8_t *payload, size_t len, uint8_t qos, bool retain, bool dup) {
		char *buf;
		_clientMQTT.xPayload(payload, len, buf);
		_onMessage(topic, buf);
		free(buf);    // DO NOT FORGET TO DO THIS!!!
	});
	_clientMQTT.onDisconnect([&](int8_t reason) {
		_onDisconnect(reason);
	});
}
bool BasicMQTT::waitForMQTT(int waitTime) {
	u_long startWaitingAt = millis();
	BASICMQTT_PRINT("Connecting MQTT");
	while (!_connected) {
		BASICMQTT_PRINT(".");
		if (BasicSetup::_useLed) {
			BasicSetup::blinkLed(100, 150);
		} else {
			delay(250);
		}
		if (millis() - startWaitingAt > waitTime * 1000) {
			BASICMQTT_PRINTLN("Can't connect to MQTT!");
			return false;
			break;
		}
	}
	return true;
}
