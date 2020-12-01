#pragma once

#include "../espBasicSetup.hpp"
#include <LittleFS.h>


class BasicFS {
  public:
	bool setup();

	BasicFS();

  private:
	bool _inclFS;

    friend class BasicSetup;
};

extern BasicFS _basicFS;
