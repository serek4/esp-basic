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


#define DEBUG_PRINTER Serial
#define BASIC_TIME_DEBUG
#define BASIC_LOGS_DEBUG

class BasicSetup {
  public:
	void begin();

	BasicSetup();

  private:
	bool _fsStarted;
	bool _inclFS;
	bool _inclServerHttp;
	bool _inclMQTT;
	bool _inclWiFi;
	bool _inclConfig;
	bool _inclOTA;
	static bool _inclTime;
	static bool _inclLogger;
	bool _staticIP;
	static bool _useLed;

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
