#pragma once

#include "../espBasicSetup.hpp"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <SPIFFSEditor.h>

class BasicServerHttp {
  public:
	void setup();

	BasicServerHttp();

  private:
	bool _inclServerHttp;

	friend class BasicSetup;
};

extern BasicServerHttp _basicServerHttp;
extern AsyncWebServer _serverHttp;
