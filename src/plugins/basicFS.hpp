#pragma once

#include "../espBasicSetup.hpp"
#include <LittleFS.h>


class BasicFS {
  public:
	static bool setup();

	BasicFS();
	~BasicFS();

  private:
	static bool _fsStarted;

	friend class BasicConfig;
	friend class BasicServerHttp;
	friend class BasicLogs;
	friend class EspBasicSetup;
};
