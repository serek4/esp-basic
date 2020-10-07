#pragma once

#include "Arduino.h"


#include "plugins/basicConfig.hpp"
#include "plugins/basicFS.hpp"
#include "plugins/basicMQTT.hpp"
#include "plugins/basicOTA.hpp"
#include "plugins/basicServerHttp.hpp"
#include "plugins/basicWiFi.hpp"


class BasicSetup {
  public:
	void begin();

	BasicSetup();

  private:
	bool _fsStarted;
	bool _inclServerHttp;
	bool _inclMQTT;
	bool _inclWiFi;
	bool _inclConfig;
	bool _inclOTA;
	bool _staticIP;
	static bool _useLed;

	friend class ImportSetup;
	friend class BasicFS;
	friend class BasicConfig;
	friend class BasicServerHttp;
	friend class BasicMQTT;
	friend class BasicOTA;
	friend class BasicWiFi;
};

extern BasicSetup _basicSetup;
