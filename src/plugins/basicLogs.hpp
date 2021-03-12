#pragma once

#include "../espBasicSetup.hpp"


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
