#include "espBasicSetup.hpp"


BasicSetup::BasicSetup()
    : _fsStarted(false)
    , _inclFS(_basicFS._inclFS)
    , _inclConfig(_basicConfig._inclConfig)
    , _inclWiFi(_basicWiFi._inclWiFi)
    , _inclOTA(_basicOTA._inclOTA)
    , _inclServerHttp(_basicServerHttp._inclServerHttp) {
	if (_useLed) {
		pinMode(LED_BUILTIN, OUTPUT);
	}
}
void BasicSetup::begin() {
	_basicConfig.setup();
	_basicOTA.setup();
	_basicServerHttp.setup();
	_basicWiFi.setup(_basicSetup._staticIP);
}

BasicSetup _basicSetup;