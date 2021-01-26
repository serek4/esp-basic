#pragma once

#include "../espBasicSetup.hpp"

// clang-format off
// Setup debug printing macros.
#ifdef BASIC_LOGS_DEBUG
#define BASICLOGS_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICLOGS_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICLOGS_PRINT(...) {}
#define BASICLOGS_PRINTLN(...) {}
#endif
// clang-format on

typedef enum {
	ll_error,
	ll_warning,
	ll_info,
	ll_log,
	ll_debug,
	ll_unknown = 99
} LogLevel;


class BasicLogs {
  private:
	static String _pendingLogs;
	static String _logBuffer;
	static u_long _saveLogDelayTimer;
	static const char *_logLevelStr[];

  public:
	static void handle();
	static void saveLog(time_t time, uint8_t logLevel, String message);

	BasicLogs();
	~BasicLogs();
};
