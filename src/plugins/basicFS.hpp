#pragma once

#include "../espBasicSetup.hpp"
#include <LittleFS.h>
#ifdef ARDUINO_ARCH_ESP32
#include <esp_littlefs.h>
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
