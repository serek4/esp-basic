#include <espBasicSetup.h>

basicSetup mySetup;

void setup() {
	mySetup.begin();
	mySetup.onMQTTconnect(handleMQTTconnect);
	mySetup.onMQTTmessage(handleIncMQTTmsg);
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
	mySetup.MQTTpublish("ESP/wemos/feedback", payload);
}
