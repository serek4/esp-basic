#include "basicServerHttp.hpp"

AsyncWebServer _serverHttp(80);

BasicServerHttp::BasicServerHttp()
    : _inclServerHttp(true) {}
void BasicServerHttp::setup() {
	if (!(_basicSetup._fsStarted)) {
		Serial.println("mount 2");
		_basicSetup._fsStarted = _basicFS.setup();
	}
	if (_basicSetup._fsStarted) {
		_serverHttp.addHandler(new SPIFFSEditor(_config.http.user, _config.http.pass, LittleFS));
		_serverHttp.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
			request->redirect("/edit");
		});
		_serverHttp.onNotFound([](AsyncWebServerRequest *request) {
			String message = "File Not Found\n\n";
			message += "\nURI: ";
			message += request->url();
			message += "\nMethod: ";
			message += request->methodToString();
			message += "\nArguments: ";
			message += request->args();
			message += "\n";
			for (uint8_t i = 0; i < request->args(); i++) {
				message += " " + request->argName(i);
				message += ": " + request->arg(i) + "\n";
			}
			message += "\n";
			request->send(404, "text/plain", message);
		});
	}
}

BasicServerHttp _basicServerHttp;
