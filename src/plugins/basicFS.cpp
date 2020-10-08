#include "basicFS.hpp"


BasicFS::BasicFS()
    : _inclFS(true) {
}

bool BasicFS::setup() {
	while (!LittleFS.begin()) {
		Serial.println("LittleFS mount failed!");
		return false;
	}
	Serial.println("LittleFS mounted!");
	return true;
}

BasicFS _basicFS;
