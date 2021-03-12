#include "basicFS.hpp"


BasicFS::BasicFS() {
	_fsStarted = setup();
}
BasicFS::~BasicFS() {
}

bool BasicFS::setup() {
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
}
