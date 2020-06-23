#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <SPIFFSEditor.h>
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
	bool FSsetup();
	void HTTPsetup();
	void begin();

	basicSetup();
	basicSetup(bool ota, bool mqtt, bool webEditor);
};
