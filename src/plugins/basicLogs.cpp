#include "basicLogs.hpp"


String BasicLogs::_pendingLogs = "";
String BasicLogs::_logBuffer = "";
u_long BasicLogs::_saveLogDelayTimer = 0;
const char *BasicLogs::_logLevelStr[] = {"error", "warning", "info", "log", "debug", "unknown"};

BasicLogs::BasicLogs() {
	if (!BasicFS::_fsStarted) {
		BASICFS_PRINTLN("mount 3");
		BasicFS::_fsStarted = BasicFS::setup();
	}
}

BasicLogs::~BasicLogs() {
}

void BasicLogs::saveLog(time_t time, uint8_t logLevel, String message) {
	constrain(logLevel, 0, 5);
	_pendingLogs += BasicTime::dateTimeString(time) + ", " + _logLevelStr[logLevel] + ", " + message + '\n';
}

void BasicLogs::handle() {
	if (_pendingLogs.length() > 0) {    // add pending logs to buffer and clear pending logs
		_logBuffer += _pendingLogs;
		_pendingLogs = "";
	}
	if (_logBuffer.length() > 0 && millis() - _saveLogDelayTimer >= 100) {    // save logs from buffer and clear it on success
		File logFile;
		if (!(LittleFS.exists("log.csv"))) {
			logFile = LittleFS.open("log.csv", "w");
			logFile.print("time, logLevel, message\n");
		} else {
			logFile = LittleFS.open("log.csv", "a");
		}
		if (!logFile) {
			BASICLOGS_PRINTLN("write file error: log.csv");
		} else {
			logFile.print(_logBuffer);
			BASICLOGS_PRINTLN("saved logs\n====>");
			BASICLOGS_PRINT(_logBuffer);
			BASICLOGS_PRINTLN("<====");
			BASICLOGS_PRINTLN("pending logs\n====>");
			BASICLOGS_PRINT(_pendingLogs);
			BASICLOGS_PRINTLN("<====");
			_logBuffer = "";
		}
		logFile.close();
		_saveLogDelayTimer = millis();
	}
}