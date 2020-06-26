#include <espBasicSetup.h>
#include <secrets.h>

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
void handleIncMQTTmsg(char *topic, char *payload) {
	Serial.printf("Incomming mqtt message!\n msg.topic:   %s\n msg.payload: %s\n", topic, payload);
	mySetup.MQTTpublish("ESP/esp8266wemos/feedback", payload);
}