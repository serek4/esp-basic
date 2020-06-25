#include "Arduino.h"

#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <SPIFFSEditor.h>
#include <Ticker.h>

void MQTTmessage(char *topic, char *payload);

class basicSetup {
  public:
	void waitForWiFi();
	void WiFiSetup(bool &waitForConnection);
	void OTAsetup();
	void MQTTsetup(bool &waitForConnection);
	void waitForMQTT();
	bool FSsetup();
	void HTTPsetup();
	void begin(bool waitForWiFi = true, bool waitForMQTT = false);

	basicSetup();
	basicSetup(bool ota, bool mqtt, bool webEditor);

  private:
	bool _fsStarted;
};
