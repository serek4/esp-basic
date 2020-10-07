#pragma once

#include "../espBasicSetup.hpp"
#include <ArduinoOTA.h>


class BasicOTA {
  public:
	void setup();

	BasicOTA();

  private:
	bool _inclOTA;

	friend class BasicSetup;
};

extern BasicOTA _basicOTA;
