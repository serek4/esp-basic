#pragma once

#include "./espBasicSetup.hpp"
#include <LittleFS.h>


class BasicFS {
  public:
	bool setup();

	BasicFS();
};

extern BasicFS _basicFS;
