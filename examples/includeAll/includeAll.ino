#include "secrets.h"
#include <espBasicSetup.h>

EspBasicSetup mySetup;
struct UserConfig {
	char *testStr = USER_STR;
	int testInt = USER_INT;
} myConfig;


void setup() {
	mySetup.config.SetUserConfigSize(JSON_OBJECT_SIZE(2) + 40);
	mySetup.config.loadUserConfig(loadConfig);
	mySetup.config.saveUserConfig(saveConfig);
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

bool loadConfig(JsonObject config) {
	bool succes = true;
	if (!mySetup.config.checkJsonVariant(myConfig.testStr, config["teststr"])) succes &= false;
	if (!mySetup.config.checkJsonVariant(myConfig.testInt, config["testint"])) succes &= false;
	return succes;
}
void saveConfig(JsonObject config) {
	config["teststr"] = myConfig.testStr;
	config["testint"] = myConfig.testInt;
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
