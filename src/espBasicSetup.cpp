#include "espBasicSetup.hpp"


BasicSetup::BasicSetup()
    : _inclWiFi(_basicWiFi._inclWiFi) {
	if (_useLed) {
		pinMode(LED_BUILTIN, OUTPUT);
	}
}
void BasicSetup::begin() {
	_basicWiFi.setup(_basicSetup._staticIP);
}

BasicSetup _basicSetup;