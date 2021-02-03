#include "basicTime.hpp"

AsyncUDP NTPudp;

bool _waitingForNTP = false;
u_long _requestSendedAt;
bool _gotNTPserverIP = false;
IPAddress _timeServerIP;                         // NTP server address
time_t NTPSyncInterval = 12 * SECS_PER_HOUR;     // timeSet sync interval
time_t NTPReSyncInterval = 1 * SECS_PER_HOUR;    // timeNeedsSync sync interval
time_t NTPnoSyncInterval = 5 * SECS_PER_MIN;     // timeNotSet sync interval

BasicTime::BasicTime() {
}

void BasicTime::setup() {
	setSyncInterval(NTPReSyncInterval);
	setSyncProvider(_requestNtpTime);
}
void BasicTime::waitForNTP(int waitTime) {
	u_long startWaitingAt = millis();
	BASICTIME_PRINT("Waiting for NTP connection");
	while (timeStatus() != timeSet) {
		BASICTIME_PRINT(".");
		handle();
		if (BasicSetup::_useLed) {
			digitalWrite(LED_BUILTIN, LOW);
			delay(100);
			digitalWrite(LED_BUILTIN, HIGH);
			delay(150);
		} else {
			delay(250);
		}
		if (millis() - startWaitingAt > waitTime * 1000) {
			BASICTIME_PRINTLN("Can't connect to NTP server!");
			break;
		}
	}
}
//request time from NTP server
time_t BasicTime::_requestNtpTime() {
	if (_gotNTPserverIP) {
		BASICTIME_PRINTLN("Syncing time with NTP");
		if (_sendNTPpacket(_timeServerIP)) {
			NTPudp.onPacket(_NTPrequestCallback);
		}
	}
	return 0;
}
// response from NTP server
void BasicTime::_NTPrequestCallback(AsyncUDPPacket &packet) {    // response packet
	// convert four bytes starting at location 40 to a long integer
	u_long secsSince1900 = (unsigned long)packet.data()[40] << 24;
	secsSince1900 |= (unsigned long)packet.data()[41] << 16;
	secsSince1900 |= (unsigned long)packet.data()[42] << 8;
	secsSince1900 |= (unsigned long)packet.data()[43];
	setTime(secsSince1900 - 2208988800UL);    // unix time - 70 years
	_NTPsyncInterval("Time synced with NTP server\nNext sync in ");
	NTPudp.close();
	_waitingForNTP = false;
}
// sync time response checker
void BasicTime::handle() {
	if (!_gotNTPserverIP && WiFi.isConnected()) {    // waiting for WiFi connection to get NTP server IP
		_gotNTPserverIP = WiFi.hostByName(_config.time.NTP_server_address, _timeServerIP);
		if (timeStatus() != timeSet) {
			_requestNtpTime();
		}
	}
	if (_waitingForNTP && millis() - _requestSendedAt >= NTP_TIMEOUT) {    // NTP packet sended waiting for response
		_NTPsyncInterval("No response from NTP server\nNext sync in ");
		_waitingForNTP = false;
	}
}
//sending UDP packet to NTP server
bool BasicTime::_sendNTPpacket(IPAddress &address) {    // send an NTP request to the time server
	if (NTPudp.connect(address, 123)) {                 // NTP requests are to port 123
		byte packetBuffer[NTP_PACKET_SIZE];             // buffer to hold outgoing packet
		memset(packetBuffer, 0, NTP_PACKET_SIZE);
		packetBuffer[0] = 0b11100011;    // LI, Version, Mode
		packetBuffer[1] = 0;             // Stratum, or type of clock
		packetBuffer[2] = 6;             // Polling Interval
		packetBuffer[3] = 0xEC;          // Peer Clock Precision
		                                 // 8 bytes of zero for Root Delay & Root Dispersion
		packetBuffer[12] = 49;
		packetBuffer[13] = 0x4E;
		packetBuffer[14] = 49;
		packetBuffer[15] = 52;
		NTPudp.write(packetBuffer, NTP_PACKET_SIZE);
		setSyncInterval(NTPReSyncInterval);    // set short sync interval when waiting for server response
		_requestSendedAt = millis();
		_waitingForNTP = true;
		return true;
	} else {
		_NTPsyncInterval("Can't connect to NTP server\nNext sync in ");
		return false;
	}
}
void BasicTime::_NTPsyncInterval(const char *message) {
	String logMessage = message;
	switch (timeStatus()) {
		case timeNotSet:
			setSyncInterval(NTPnoSyncInterval);    // set very short sync interval if time was never synced
			logMessage += (String)(NTPnoSyncInterval / SECS_PER_MIN) + "m";
			break;
		case timeNeedsSync:
			setSyncInterval(NTPReSyncInterval);    // set short sync interval if time was set at least once
			logMessage += (String)(NTPReSyncInterval / SECS_PER_HOUR) + "h";
			break;
		case timeSet:
			setSyncInterval(NTPSyncInterval);    // set long sync interval on successful sync
			logMessage += (String)(NTPSyncInterval / SECS_PER_HOUR) + "h";
			break;

		default:
			break;
	}
	BASICTIME_PRINTLN(logMessage);
	logMessage.replace("\n", ". ");
	if (BasicSetup::_inclLogger) {
		BasicLogs::saveLog(now(), ll_log, logMessage);
	}
}
//converting timestamp to human readable date string (RRRR-MM-DD)
String BasicTime::dateString(time_t timestamp) {
	String date = "timeNotSet";
	if (timeStatus() != timeNotSet) {
		timestamp += _config.time.timezone * SECS_PER_HOUR;             // time zone +1h
		if (_config.time.summertime) timestamp += 1 * SECS_PER_HOUR;    // summer time +1h
		date = (String)(year(timestamp));
		date += '-';
		if (month(timestamp) < 10) date += '0';
		date += (String)(month(timestamp));
		date += '-';
		if (day(timestamp) < 10) date += '0';
		date += (String)(day(timestamp));
	}
	return date;
}
//converting timestamp to human readable time string (hh:mm:ss)[24h]
String BasicTime::timeString(time_t timestamp) {
	String time = "";
	if (timeStatus() != timeNotSet) {
		timestamp += _config.time.timezone * SECS_PER_HOUR;             // time zone +1h
		if (_config.time.summertime) timestamp += 1 * SECS_PER_HOUR;    // summer time +1h
	}
	if (hour(timestamp) < 10) time += '0';
	time += (String)hour(timestamp);
	time += ':';
	if (minute(timestamp) < 10) time += '0';
	time += (String)(minute(timestamp));
	time += ':';
	if (second(timestamp) < 10) time += '0';
	time += (String)(second(timestamp));
	if (timeStatus() == timeNeedsSync) {
		time += "*";
	}
	return time;
}
//converting timestamp to human readable date time string (RRRR-MM-DD hh:mm:ss)[24h]
String BasicTime::dateTimeString(time_t timestamp) {
	return dateString(timestamp) + ' ' + timeString(timestamp);
}
