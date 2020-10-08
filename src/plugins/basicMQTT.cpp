#include "basicMQTT.hpp"


PangolinMQTT _clientMQTT;
Ticker _mqttReconnectTimer;

BasicMQTT::BasicMQTT()
    : _inclMQTT(true) {
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
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)payload, (size_t)strlen(payload), false);
}
void BasicMQTT::publish(const char *topic, int payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	itoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), false);
}
void BasicMQTT::publish(const char *topic, uint16_t payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	utoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), false);
}
void BasicMQTT::publish(const char *topic, long payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	ltoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), false);
}
void BasicMQTT::publish(const char *topic, u_long payload, uint8_t qos, bool retain) {
	char numberBuffer[12];
	ultoa(payload, numberBuffer, 10);
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), false);
}
void BasicMQTT::publish(const char *topic, float payload, signed char width, unsigned char prec, uint8_t qos, bool retain) {
	char numberBuffer[12];
	dtostrf(payload, width, prec, numberBuffer);
	_clientMQTT.publish(topic, qos, retain, (uint8_t *)numberBuffer, (size_t)strlen(numberBuffer), false);
}
uint16_t BasicMQTT::subscribe(const char *topic, uint8_t qos) {
	return _clientMQTT.subscribe(topic, qos);
}
bool BasicMQTT::connected() {
	_clientMQTT.connected();
}

void BasicMQTT::_onConnect() {
	Serial.println((String) "MQTT connected!\n " + _clientMQTT.getClientId() + "@" + _config.mqtt.broker);
	uint16_t subStatus = _clientMQTT.subscribe(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), 2);
	uint16_t subCommands = _clientMQTT.subscribe(((String) "ESP/" + _clientMQTT.getClientId() + "/commands").c_str(), 2);
	_clientMQTT.publish(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), 2, true, (uint8_t *)"on", strlen("on"), false);
	for (const auto &handler : _onConnectHandler) handler();
}
void BasicMQTT::_onMessage(const char *_topic, const char *_payload) {
	if (strcmp(_topic, _config.mqtt.will_topic) == 0) {
		if (strcmp(_payload, _config.mqtt.will_msg) == 0) {
			_clientMQTT.publish(((String) "ESP/" + _clientMQTT.getClientId() + "/status").c_str(), 2, true, (uint8_t *)"on", strlen("on"), false);
		}
	}
	for (const auto &handler : _onMessageHandler) handler(_topic, _payload);
}
void BasicMQTT::_onDisconnect(int8_t reason) {
	Serial.println("MQTT disconnected: [" + String(reason, 10) + "]!");
	_mqttReconnectTimer.once(10, []() { _clientMQTT.connect(); });
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
	_clientMQTT.onMessage([&](const char *topic, uint8_t *payload, PANGO_PROPS_t properties, size_t len, size_t index, size_t total) {
		char fixedPayload[len + 1];
		fixedPayload[len] = '\0';
		strncpy(fixedPayload, PANGO::payloadToCstring(payload, len), len);
		_onMessage(topic, fixedPayload);
	});
	_clientMQTT.onDisconnect([&](int8_t reason) {
		_onDisconnect(reason);
	});
}
void BasicMQTT::waitForMQTT(int waitTime) {
	u_long startWaitingAt = millis();
	Serial.print("Connecting MQTT");
	while (!_clientMQTT.connected()) {
		Serial.print(".");
		if (BasicSetup::_useLed) {
			digitalWrite(LED_BUILTIN, LOW);
			delay(100);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(150);
		} else {
			delay(250);
		}
		if (millis() - startWaitingAt > waitTime * 1000) {
			Serial.println("Can't connect to MQTT!");
			break;
		}
	}
}

BasicMQTT _basicMQTT;
