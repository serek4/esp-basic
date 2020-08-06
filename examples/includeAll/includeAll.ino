#include "secrets.h"
#include <espBasicSetup.h>

EspBasicSetup mySetup;

void setup() {
	mySetup.begin();
	MQTT.onConnect(handleMQTTconnect);
	MQTT.onMessage(handleIncMQTTmsg);
}

void loop() {
	ArduinoOTA.handle();
	delay(10);
}

void handleMQTTconnect() {
	Serial.println("User handler for MQTT onConnect");
}
void handleIncMQTTmsg(const char *topic, const char *payload) {
	Serial.printf("Incomming mqtt message!\n msg.topic:   %s\n msg.payload: %s\n", topic, payload);
	MQTT.publish("ESP/wemos/feedback", payload);
}
