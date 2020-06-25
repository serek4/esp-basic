#include "Arduino.h"
#include "secrets.h"

#include <ArduinoJson.h>
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
	class Config {
	  public:
		// WiFi settings
		class WiFi {
		  public:
			char ssid[32];
			char pass[64];
			WiFiMode mode;
#if STATIC_IP
			IPAddress IP;         // optional
			IPAddress subnet;     // optional
			IPAddress gateway;    // optional
			IPAddress DNS1;       // optional
			IPAddress DNS2;       // optional
#endif
			WiFi();
		};
		// OTA settings
		class OTA {
		  public:
			char hostname[32];
			OTA();
		};
		// MQTT settings
		class MQTT {
		  public:
			char broker[32];
			int broker_port;
			char client_ID[32];
			int keepalive;
#if MQTT_SET_LASTWILL
			char will_topic[64];    // optional
			char will_msg[16];      // optional
#endif
#if MQTT_USE_CREDENTIALS
			char user[16];    // optional
			char pass[16];    // optional
#endif
			MQTT();
		};
		// web file editor
		class HTTP {
		  public:
			char user[16];
			char pass[16];
			HTTP();
		};
		WiFi wifi;
		OTA ota;
		MQTT mqtt;
		HTTP http;
		void saveConfig();
		bool loadConfig();
	};
	void waitForWiFi();
	void WiFiSetup(bool &waitForConnection);
	void OTAsetup();
	void MQTTsetup(bool &waitForConnection);
	void waitForMQTT();
	bool FSsetup();
	void HTTPsetup();
	void begin(bool waitForWiFi = true, bool waitForMQTT = false);

	basicSetup();
	basicSetup(bool inclConfigFile, bool ota, bool mqtt, bool webEditor);

  private:
	bool _fsStarted = false;
};
