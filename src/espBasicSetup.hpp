#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>

void MQTTmessage(char* topic, char* payload);

class basicSetup {
  public:
	void waitForWiFi();
	void WiFiSetup();
	void OTAsetup();
	void MQTTsetup();
	void waitForMQTT();
	void begin();

	basicSetup();
	basicSetup(bool ota, bool mqtt);
};
