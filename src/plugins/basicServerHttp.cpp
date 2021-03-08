#include "basicServerHttp.hpp"

AsyncWebServer _serverHttp(80);

BasicServerHttp::BasicServerHttp()
    : _inclServerHttp(true) {
}

void BasicServerHttp::setup() {
	if (!(BasicFS::_fsStarted)) {
		BASICFS_PRINTLN("mount 2");
		BasicFS::_fsStarted = BasicFS::setup();
	}
	if (BasicFS::_fsStarted) {
		_serverHttp.addHandler(new SPIFFSEditor(BasicConfig::configFile.http.user, BasicConfig::configFile.http.pass, LittleFS));
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
