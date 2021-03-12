#pragma once

#include "../espBasicSetup.hpp"
#include <ArduinoOTA.h>

// clang-format off
// Setup debug printing macros.
#ifdef BASIC_OTA_DEBUG
#define BASICOTA_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICOTA_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#define BASICOTA_PRINTF(...) { DEBUG_PRINTER.printf(__VA_ARGS__); }
#else
#define BASICOTA_PRINT(...) {}
#define BASICOTA_PRINTLN(...) {}
#define BASICOTA_PRINTF(...) {}
#endif
// clang-format on

class BasicOTA {
  public:
	void setup();

	BasicOTA();
	BasicOTA(const char *hostname);
	~BasicOTA();

  private:
	static char _hostname[32];

	friend class BasicConfig;
};
