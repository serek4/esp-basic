#pragma once

#include "../espBasicSetup.hpp"
#include <ESPAsyncWebServer.h>
#ifdef ARDUINO_ARCH_ESP32
#include <LITTLEFS.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <LittleFS.h>
#endif
#include <SPIFFSEditor.h>


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
