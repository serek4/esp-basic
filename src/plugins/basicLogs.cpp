#include "basicLogs.hpp"


String BasicLogs::_pendingLogs = "";
const char *BasicLogs::_logLevelStr[] = {"error", "warning", "info", "log", "debug", "unknown"};

BasicLogs::BasicLogs() {
}

BasicLogs::~BasicLogs() {
}

void BasicLogs::saveLog(time_t time, uint8_t logLevel, String message) {
	constrain(logLevel, 0, 5);
	_pendingLogs += BasicTime::dateTimeString(time) + ", " + _logLevelStr[logLevel] + ", " + message + '\n';
}

void BasicLogs::handle() {
	if (_pendingLogs.length() > 0) {
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
			logFile.print(_pendingLogs);
			BASICLOGS_PRINTLN("saved logs\nbegin====>");
			BASICLOGS_PRINTLN(_pendingLogs);
			BASICLOGS_PRINTLN("<====end");
			_pendingLogs = "";
		}
		logFile.close();
	}
}