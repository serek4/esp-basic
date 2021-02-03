#include "basicMQTT.hpp"


PangolinMQTT _clientMQTT;
Ticker _mqttReconnectTimer;

BasicMQTT::BasicMQTT()
    : _inclMQTT(true)
    , _connected(false) {
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
	BASICMQTT_PRINTLN((String) "MQTT connected!\n " + _clientMQTT.getClientId() + "@" + _config.mqtt.broker);
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_debug, (String) "MQTT connected [" + _clientMQTT.getClientId() + "@" + _config.mqtt.broker + "]");
	}
	_connected = true;
	_clientMQTT.subscribe(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), 2);
	_clientMQTT.subscribe(((String) "ESP/" + _clientMQTT.getClientId() + "/commands").c_str(), 2);
	_clientMQTT.publish(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), (uint8_t *)"on", strlen("on"), 2, true);
	for (const auto &handler : _onConnectHandler) handler();
}
void BasicMQTT::_onMessage(const char *_topic, const char *_payload) {
	if (strcmp(_topic, _config.mqtt.will_topic) == 0) {
		if (strcmp(_payload, _config.mqtt.will_msg) == 0) {
			_clientMQTT.publish(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), "on", strlen("on"), 2, true);
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
	_connected = false;
	_mqttReconnectTimer.once(_config.mqtt.keepalive * PANGO_POLL_RATE, []() { _clientMQTT.connect(); });
	for (const auto &handler : _onDisconnectHandler) handler(reason);
}
void BasicMQTT::setup() {
	//TODO sprintf(_config.mqtt.client_ID, "esp8266-%06x", ESP.getChipId());
	_clientMQTT.setClientId(_config.mqtt.client_ID);
	_clientMQTT.setKeepAlive(_config.mqtt.keepalive);
	_clientMQTT.setWill(_config.mqtt.will_topic, 2, true, _config.mqtt.will_msg);
	_clientMQTT.setCredentials(_config.mqtt.user, _config.mqtt.pass);
	_clientMQTT.setServer(_config.mqtt.broker, _config.mqtt.broker_port);
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
void BasicMQTT::waitForMQTT(int waitTime) {
	u_long startWaitingAt = millis();
	BASICMQTT_PRINT("Connecting MQTT");
	while (!_connected) {
		BASICMQTT_PRINT(".");
		if (BasicSetup::_useLed) {
			digitalWrite(LED_BUILTIN, LOW);
			delay(100);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(150);
		} else {
			delay(250);
		}
		if (millis() - startWaitingAt > waitTime * 1000) {
			BASICMQTT_PRINTLN("Can't connect to MQTT!");
			break;
		}
	}
}

BasicMQTT _basicMQTT;
