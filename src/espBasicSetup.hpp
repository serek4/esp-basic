#pragma once

#include "Arduino.h"


#include "plugins/basicConfig.hpp"
#include "plugins/basicFS.hpp"
#include "plugins/basicLogs.hpp"
#include "plugins/basicMQTT.hpp"
#include "plugins/basicOTA.hpp"
#include "plugins/basicServerHttp.hpp"
#include "plugins/basicTime.hpp"
#include "plugins/basicWiFi.hpp"


// #define DEBUG_MESSAGES

#ifdef DEBUG_MESSAGES
#define DEBUG_PRINTER Serial
#define BASIC_FS_DEBUG
#define BASIC_CONFIG_DEBUG
#define BASIC_WIFI_DEBUG
#define BASIC_OTA_DEBUG
#define BASIC_MQTT_DEBUG
#define BASIC_SERVERHTTP_DEBUG
#define BASIC_TIME_DEBUG
#define BASIC_LOGS_DEBUG
#endif

class BasicSetup {
  public:
	void begin();

	BasicSetup();

  private:
	static bool _inclFS;
	bool _inclServerHttp;
	static bool _inclMQTT;
	bool _inclWiFi;
	bool _inclConfig;
	bool _inclOTA;
	static bool _inclTime;
	static bool _inclLogger;
	bool _staticIP;
	static bool _useLed;

	friend class EspBasicSetup;
	friend class ImportSetup;
	friend class BasicFS;
	friend class BasicConfig;
	friend class BasicServerHttp;
	friend class BasicMQTT;
	friend class BasicOTA;
	friend class BasicWiFi;
	friend class BasicTime;
	friend class BasicLogger;
};

extern BasicSetup _basicSetup;
