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
		BASICFS_PRINTLN("LittleFS already mounted!");
		return true;
	}
	if (!LITTLEFS.begin()) {
		BASICFS_PRINTLN("LITTLEFS mount failed!");
		return false;
	}
	BASICFS_PRINTLN("LITTLEFS mounted!");
	return true;
#elif defined(ARDUINO_ARCH_ESP8266)
	FSInfo FsInfo;
	if (LittleFS.info(FsInfo)) {
		BASICFS_PRINTLN("LittleFS already mounted!");
		return true;
	}
	while (!LittleFS.begin()) {
		BASICFS_PRINTLN("LittleFS mount failed!");
		return false;
	}
	BASICFS_PRINTLN("LittleFS mounted!");
	return true;
#endif
}
