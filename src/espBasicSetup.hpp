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

// clang-format off
// Setup debug printing macros for config plugin.
#ifdef BASIC_CONFIG_DEBUG
#define BASICCONFIG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICCONFIG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICCONFIG_PRINT(...) {}
#define BASICCONFIG_PRINTLN(...) {}
#endif
// Setup debug printing macros for file system plugin.
#ifdef BASIC_FS_DEBUG
#define BASICFS_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICFS_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICFS_PRINT(...) {}
#define BASICFS_PRINTLN(...) {}
#endif
// Setup debug printing macros for logger plugin.
#ifdef BASIC_LOGS_DEBUG
#define BASICLOGS_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICLOGS_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICLOGS_PRINT(...) {}
#define BASICLOGS_PRINTLN(...) {}
#endif
// Setup debug printing macros for mqtt plugin.
#ifdef BASIC_MQTT_DEBUG
#define BASICMQTT_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICMQTT_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICMQTT_PRINT(...) {}
#define BASICMQTT_PRINTLN(...) {}
#endif
// Setup debug printing macros for OTA plugin.
#ifdef BASIC_OTA_DEBUG
#define BASICOTA_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICOTA_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#define BASICOTA_PRINTF(...) { DEBUG_PRINTER.printf(__VA_ARGS__); }
#else
#define BASICOTA_PRINT(...) {}
#define BASICOTA_PRINTLN(...) {}
#define BASICOTA_PRINTF(...) {}
#endif
// Setup debug printing macros for http server plugin.
#ifdef BASIC_SERVERHTTP_DEBUG
#define BASICSERVERHTTP_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICSERVERHTTP_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICSERVERHTTP_PRINT(...) {}
#define BASICSERVERHTTP_PRINTLN(...) {}
#endif
// Setup debug printing macros for ntp time plugin.
#ifdef BASIC_TIME_DEBUG
#define BASICTIME_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICTIME_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICTIME_PRINT(...) {}
#define BASICTIME_PRINTLN(...) {}
#endif
// Setup debug printing macros for wifi plugin.
#ifdef BASIC_WIFI_DEBUG
#define BASICWIFI_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICWIFI_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICWIFI_PRINT(...) {}
#define BASICWIFI_PRINTLN(...) {}
#endif
// clang-format on


class BasicSetup {
  public:
	void begin();

	BasicSetup();

  private:
	static bool _inclFS;
	static bool _inclServerHttp;
	static bool _inclMQTT;
	static bool _inclWiFi;
	static bool _inclConfig;
	static bool _inclOTA;
	static bool _inclTime;
	static bool _inclLogger;
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
