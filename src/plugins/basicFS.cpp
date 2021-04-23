#include "basicFS.hpp"


BasicFS::BasicFS() {
#if defined(ARDUINO_ARCH_ESP8266)
	_fsStarted = setup();    //* crash on esp32
#endif
}
BasicFS::~BasicFS() {
}

bool BasicFS::setup() {
#ifdef ARDUINO_ARCH_ESP32
	if (esp_littlefs_mounted("spiffs")) {
#elif defined(ARDUINO_ARCH_ESP8266)
	FSInfo FsInfo;
	if (FILE_SYSTEM.info(FsInfo)) {
#endif
		BASICFS_PRINTLN("file system already mounted!");
		return true;
	}
	while (!FILE_SYSTEM.begin()) {
		BASICFS_PRINTLN("file system mount failed!");
		return false;
	}
	BASICFS_PRINTLN("file system mounted!");
	return true;
}
String BasicFS::fileName(String filename) {
#ifdef ARDUINO_ARCH_ESP32
	if(!filename.startsWith("/")) {
		return "/" + filename;
	}
#endif
	return filename;
}

