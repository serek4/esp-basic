#include "basicServerHttp.hpp"

AsyncWebServer _serverHttp(80);

char BasicServerHttp::_user[16];
char BasicServerHttp::_pass[16];

BasicServerHttp::BasicServerHttp() {
	strcpy(_user, "admin");
	strcpy(_pass, "admin");
}
BasicServerHttp::BasicServerHttp(const char *user, const char *pass) {
	strcpy(_user, user);
	strcpy(_pass, pass);
}
BasicServerHttp::~BasicServerHttp() {
}

void BasicServerHttp::setup() {
	if (!(BasicFS::_fsStarted)) {
		BASICFS_PRINTLN("mount 2");
		BasicFS::_fsStarted = BasicFS::setup();
	} else {
		BASICFS_PRINTLN("mount 2 skipped");
	}
	if (BasicFS::_fsStarted) {
#ifdef ARDUINO_ARCH_ESP32
		_serverHttp.addHandler(new SPIFFSEditor(FILE_SYSTEM, _user, _pass));
#elif defined(ARDUINO_ARCH_ESP8266)
		_serverHttp.addHandler(new SPIFFSEditor(_user, _pass, FILE_SYSTEM));
#endif
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
