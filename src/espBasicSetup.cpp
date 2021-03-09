#include "espBasicSetup.hpp"


BasicSetup::BasicSetup() {
	if (_useLed) {
		pinMode(LED_BUILTIN, OUTPUT);
	}
}
void BasicSetup::begin() {
}

BasicSetup _basicSetup;