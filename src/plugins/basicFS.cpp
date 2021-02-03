#include "basicFS.hpp"


BasicFS::BasicFS()
    : _inclFS(true) {
}

bool BasicFS::setup() {
	while (!LittleFS.begin()) {
		BASICFS_PRINTLN("LittleFS mount failed!");
		return false;
	}
	BASICFS_PRINTLN("LittleFS mounted!");
	return true;
}

BasicFS _basicFS;
