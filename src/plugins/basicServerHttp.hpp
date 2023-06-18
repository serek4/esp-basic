#pragma once

#include "../espBasicSetup.hpp"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <SPIFFSEditor.h>


class BasicServerHttp {
  public:
	void setup();
	void syncTime();
	void reconnectMQTT();
	void reconnectWiFi();

	BasicServerHttp();
	BasicServerHttp(const char *user, const char *pass);
	~BasicServerHttp();

  private:
	static char _user[16];
	static char _pass[16];

	friend class BasicConfig;
};

extern AsyncWebServer _serverHttp;
