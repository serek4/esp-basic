#pragma once

#include "./espBasicSetup.hpp"
#include <PangolinMQTT.h>
#include <Ticker.h>
#include <functional>
#include <vector>


namespace MQTTuserHandlers {
typedef std::function<void()> onMQTTconnectHandler;
typedef std::function<void(const char *_topic, const char *_payload)> onMQTTmesageHandler;
typedef std::function<void(int8_t reason)> onMQTTdisconnectHandler;
}    // namespace MQTTuserHandlers

class BasicMQTT {
  public:
	void setup();
	void waitForMQTT(int waitTime = 10);
	void onConnect(const MQTTuserHandlers::onMQTTconnectHandler &handler);
	void onMessage(const MQTTuserHandlers::onMQTTmesageHandler &handler);
	void onDisconnect(const MQTTuserHandlers::onMQTTdisconnectHandler &handler);
	void publish(const char *topic, const char *payload, uint8_t qos = 0, bool retain = false);
	void publish(const char *topic, int payload, uint8_t qos = 0, bool retain = false);
	void publish(const char *topic, uint8_t payload, uint8_t qos = 0, bool retain = false) { publish(topic, (int)payload, qos, retain); };
	void publish(const char *topic, uint16_t payload, uint8_t qos = 0, bool retain = false);
	void publish(const char *topic, long payload, uint8_t qos = 0, bool retain = false);
	void publish(const char *topic, u_long payload, uint8_t qos = 0, bool retain = false);
	void publish(const char *topic, float payload, uint8_t qos = 0, bool retain = false) { publish(topic, payload, 3, 2, qos, retain); };
	void publish(const char *topic, float payload, signed char width, unsigned char prec, uint8_t qos = 0, bool retain = false);
	uint16_t subscribe(const char *topic, uint8_t qos = 0);

	BasicMQTT();

  private:
	bool _inclMQTT;
	std::vector<MQTTuserHandlers::onMQTTconnectHandler> _onConnectHandler;
	std::vector<MQTTuserHandlers::onMQTTmesageHandler> _onMessageHandler;
	std::vector<MQTTuserHandlers::onMQTTdisconnectHandler> _onDisconnectHandler;
	void _onConnect();
	void _onMessage(const char *_topic, const char *_payload);
	void _onDisconnect(int8_t reason);

	friend class BasicWiFi;
};

extern BasicMQTT _basicMQTT;
extern PangolinMQTT _clientMQTT;
extern Ticker _mqttReconnectTimer;
