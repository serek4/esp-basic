#include "espBasicSetup.hpp"


BasicSetup::BasicSetup()
    : _fsStarted(false)
    , _inclConfig(false)
    , _inclWiFi(false)
    , _inclOTA(false)
    , _inclMQTT(false)
    , _inclServerHttp(false) {
	if (_useLed) {
		pinMode(LED_BUILTIN, OUTPUT);
	}
}
void BasicSetup::begin() {
	_basicConfig.setup();
	_basicOTA.setup();
	_basicWiFi.setup(_basicSetup._staticIP);
	_basicMQTT.setup();
	_basicServerHttp.setup();
}

BasicSetup _basicSetup;