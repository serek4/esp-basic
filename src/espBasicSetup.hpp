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

namespace UserHandlers {
typedef std::function<void()> onMQTTconnectHandler;
typedef std::function<void(char *_topic, char *_payload)> onMQTTmesageHandler;
}    // namespace UserHandlers

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
		size_t createConfig(String filename = "config.json", bool save = true);
		bool loadConfig(String filename = "config.json");
	};
	void waitForWiFi();
	void WiFiSetup(bool &waitForConnection);
	void OTAsetup();
	void MQTTsetup(bool &waitForConnection);
	void waitForMQTT();
	void onMQTTconnect(const UserHandlers::onMQTTconnectHandler &handler);
	void onMQTTmessage(const UserHandlers::onMQTTmesageHandler &handler);
	uint16_t MQTTpublish(const char *topic, const char *payload, uint8_t qos = 0, bool retain = false);
	uint16_t MQTTsubscribe(const char *topic, uint8_t qos = 0);
	bool FSsetup();
	void HTTPsetup();
	void begin(bool waitForWiFi = true, bool waitForMQTT = false);

	basicSetup();
	basicSetup(bool inclConfigFile, bool ota, bool mqtt, bool webEditor);

  private:
	std::vector<UserHandlers::onMQTTconnectHandler> _onConnectHandler;
	std::vector<UserHandlers::onMQTTmesageHandler> _onMessageHandler;
	bool _fsStarted;

	void _onMQTTconnect();
	void _onMQTTmessage(char *_topic, char *_payload);
};
