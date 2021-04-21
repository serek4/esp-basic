#pragma once

#include "../espBasicSetup.hpp"
#include <ArduinoOTA.h>


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
