#include "espBasicSetup.hpp"


BasicSetup::BasicSetup()
    : _inclWiFi(_basicWiFi._inclWiFi)
    , _inclServerHttp(_basicServerHttp._inclServerHttp) {
	if (_useLed) {
		pinMode(LED_BUILTIN, OUTPUT);
	}
}
void BasicSetup::begin() {
	_basicServerHttp.setup();
	_basicWiFi.setup(_basicSetup._staticIP);
}

BasicSetup _basicSetup;