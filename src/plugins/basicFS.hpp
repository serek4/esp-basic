#pragma once

#include "../espBasicSetup.hpp"
#ifdef ARDUINO_ARCH_ESP32
#include <LITTLEFS.h>
#include <esp_littlefs.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <LittleFS.h>
#endif


class BasicFS {
  public:
	static bool setup();
	static String fileName(String filename);

	BasicFS();
	~BasicFS();

  private:
	static bool _fsStarted;

	friend class BasicConfig;
	friend class BasicServerHttp;
	friend class BasicLogs;
	friend class EspBasicSetup;
};
