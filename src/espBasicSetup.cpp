#include "espBasicSetup.hpp"


BasicSetup::BasicSetup() {
	if (_useLed) {
		pinMode(_ledPin, OUTPUT);
	}
}
void BasicSetup::begin() {
}

void BasicSetup::blinkLed(int onTime, int offTime) {
	digitalWrite(_ledPin, _ledON);
	delay(onTime);
	digitalWrite(_ledPin, !_ledON);
	delay(offTime);
}
BasicSetup _basicSetup;
