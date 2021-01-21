#include "basicOTA.hpp"


BasicOTA::BasicOTA()
    : _inclOTA(true) {
}

void BasicOTA::setup() {
	// TODO sprintf(_config.ota.hostname, "esp8266-%06x", ESP.getChipId());
	ArduinoOTA.setHostname(_config.ota.hostname);
	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else {    // U_FS
			type = "filesystem";
		}
		// NOTE: if updating FS this would be the place to unmount FS using FS.end()
		BASICOTA_PRINTLN("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		BASICOTA_PRINTLN("\nEnd");
		if (BasicSetup::_inclLogger) {
			BasicLogs::saveLog(now(), ll_log, "firmware update");
			BasicLogs::handle();
		}
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		BASICOTA_PRINTF("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		BASICOTA_PRINTF("Error[%u]: ", error);
		if (BasicSetup::_inclLogger) {
			BasicLogs::saveLog(now(), ll_log, "firmware update failed " + String(error, 10));
		}
		if (error == OTA_AUTH_ERROR) {
			BASICOTA_PRINTLN("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			BASICOTA_PRINTLN("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			BASICOTA_PRINTLN("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			BASICOTA_PRINTLN("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			BASICOTA_PRINTLN("End Failed");
		}
	});
}

BasicOTA _basicOTA;
