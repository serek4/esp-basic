#pragma once

#include "../espBasicSetup.hpp"
#include <LittleFS.h>

// clang-format off
// Setup debug printing macros.
#ifdef BASIC_FS_DEBUG
#define BASICFS_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICFS_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICFS_PRINT(...) {}
#define BASICFS_PRINTLN(...) {}
#endif
// clang-format on

class BasicFS {
  public:
	static bool setup();

	BasicFS();
	~BasicFS();

  private:
	static bool _fsStarted;

	friend class BasicConfig;
	friend class BasicServerHttp;
	friend class EspBasicSetup;
};
