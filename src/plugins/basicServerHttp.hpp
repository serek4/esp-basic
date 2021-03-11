#pragma once

#include "../espBasicSetup.hpp"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <SPIFFSEditor.h>

// clang-format off
// Setup debug printing macros.
#ifdef BASIC_SERVERHTTP_DEBUG
#define BASICSERVERHTTP_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICSERVERHTTP_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICSERVERHTTP_PRINT(...) {}
#define BASICSERVERHTTP_PRINTLN(...) {}
#endif
// clang-format on

class BasicServerHttp {
  public:
	void setup();

	BasicServerHttp();
	BasicServerHttp(const char *user, const char *pass);
	~BasicServerHttp();

  private:
	static char _user[16];
	static char _pass[16];

	friend class BasicConfig;
};

extern AsyncWebServer _serverHttp;
