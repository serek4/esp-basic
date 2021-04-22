#include "secrets.h"
#include <espBasicSetup.h>

EspBasicSetup mySetup;
struct UserConfig {
	String testStr = USER_STR;
	int testInt = USER_INT;
} myConfig;

long loopDelay = -50000;

void setup() {
	Serial.begin(115200);
	Serial.println("");
	mySetup.config.SetUserConfigSize(USER_CONFIG_SIZE);
	mySetup.config.loadUserConfig(loadConfig);
	mySetup.config.saveUserConfig(saveConfig);
	MQTT.onConnect(handleMQTTconnect);
	MQTT.onMessage(handleIncMQTTmsg);
	MQTT.onDisconnect(handleMQTTdisconnect);
	WIFI.onConnected(handleWiFiConnected);
	WIFI.onGotIP(handleWiFiGotIP);
	WIFI.onDisconnected(handleWiFiDisconnected);
	mySetup.begin();
	if (WIFI.waitForWiFi() == wifi_connected) {
		NTPclient.waitForNTP();
		MQTT.waitForMQTT();
	}
}

void loop() {
	ArduinoOTA.handle();
	NTPclient.handle();
	if (millis() - loopDelay >= 60000) {
		logger.saveLog(now(), ll_debug, NTPclient.dateTimeString(now()));
		loopDelay = millis();
	}
	logger.handle();
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
