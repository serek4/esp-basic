#include "secrets.h"
#include <espBasicSetup.h>

EspBasicSetup mySetup;

void setup() {
	basicSetup.userConfig.SetUserConfigSize(JSON_OBJECT_SIZE(2) + 40);
	basicSetup.userConfig.loadUserConfig(loadConfig);
	basicSetup.userConfig.saveUserConfig(saveConfig);
	mySetup.begin();
	WIFI.onConnected(handleWiFiConnected);
	WIFI.onGotIP(handleWiFiGotIP);
	WIFI.onDisconnected(handleWiFiDisconnected);
	MQTT.onConnect(handleMQTTconnect);
	MQTT.onMessage(handleIncMQTTmsg);
	MQTT.onDisconnect(handleMQTTdisconnect);
}

void loop() {
	ArduinoOTA.handle();
	delay(10);
}

void loadConfig(JsonObject config) {
	Serial.println(config["teststr"].as<char *>());
	Serial.println(config["testint"].as<int>());
}
void saveConfig(JsonObject config) {
	config["teststr"] = "test";
	config["testint"] = 22;
}
void handleWiFiConnected(const WiFiEventStationModeConnected &evt) {
	Serial.println("User handler for WIFI onConnected");
}
void handleWiFiGotIP(const WiFiEventStationModeGotIP &evt) {
	Serial.println("User handler for WIFI onGotIP");
}
void handleWiFiDisconnected(const WiFiEventStationModeDisconnected &evt) {
	Serial.println("User handler for WIFI onDisconnected");
}

void handleMQTTconnect() {
	Serial.println("User handler for MQTT onConnect");
}
void handleIncMQTTmsg(const char *topic, const char *payload) {
	Serial.printf("Incomming mqtt message!\n msg.topic:   %s\n msg.payload: %s\n", topic, payload);
	MQTT.publish("ESP/wemos/feedback", payload);
}
void handleMQTTdisconnect(int8_t reason) {
	Serial.println("User handler for MQTT onDisconnect");
}
