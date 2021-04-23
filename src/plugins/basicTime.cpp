#include "basicTime.hpp"

AsyncUDP NTPudp;

char BasicTime::_NTP_server_address[32];
int BasicTime::_NTP_server_port;
int BasicTime::_timezone;
bool BasicTime::_waitingForNTP = false;
u_long BasicTime::_requestSendedAt;
bool BasicTime::_gotNTPserverIP = false;
IPAddress BasicTime::_timeServerIP;                          // NTP server address
time_t BasicTime::_NTPSyncInterval = 12 * SECS_PER_HOUR;     // timeSet sync interval
time_t BasicTime::_NTPReSyncInterval = 1 * SECS_PER_HOUR;    // timeNeedsSync sync interval
time_t BasicTime::_NTPnoSyncInterval = 5 * SECS_PER_MIN;     // timeNotSet sync interval

BasicTime::BasicTime(const char *NTP_server_address, int NTP_server_port, int timezone) {
	strcpy(_NTP_server_address, NTP_server_address);
	_NTP_server_port = NTP_server_port;
	_timezone = timezone;
}
BasicTime::~BasicTime() {
}

void BasicTime::setup() {
	setSyncInterval(_NTPReSyncInterval);
	setSyncProvider(_requestNtpTime);
}
bool BasicTime::waitForNTP(int waitTime) {
	u_long startWaitingAt = millis();
	BASICTIME_PRINT("Waiting for NTP connection");
	while (timeStatus() != timeSet) {
		BASICTIME_PRINT(".");
		handle();
		if (BasicSetup::_useLed) {
			BasicSetup::blinkLed(100, 150);
		} else {
			delay(250);
		}
		if (millis() - startWaitingAt > waitTime * 1000) {
			BASICTIME_PRINTLN("Can't connect to NTP server!");
			return false;
			break;
		}
	}
	return true;
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
		_gotNTPserverIP = WiFi.hostByName(_NTP_server_address, _timeServerIP);
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
		setSyncInterval(_NTPReSyncInterval);    // set short sync interval when waiting for server response
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
			setSyncInterval(_NTPnoSyncInterval);    // set very short sync interval if time was never synced
			logMessage += (String)(_NTPnoSyncInterval / SECS_PER_MIN) + "m";
			break;
		case timeNeedsSync:
			setSyncInterval(_NTPReSyncInterval);    // set short sync interval if time was set at least once
			logMessage += (String)(_NTPReSyncInterval / SECS_PER_HOUR) + "h";
			break;
		case timeSet:
			setSyncInterval(_NTPSyncInterval);    // set long sync interval on successful sync
			logMessage += (String)(_NTPSyncInterval / SECS_PER_HOUR) + "h";
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
		if (isDST(timestamp)) timestamp += 1 * SECS_PER_HOUR;    // summer time +1h
		timestamp += _timezone * SECS_PER_HOUR;                  // time zone + 1h
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
		if (isDST(timestamp)) timestamp += 1 * SECS_PER_HOUR;    // summer time + 1h
		timestamp += _timezone * SECS_PER_HOUR;                  // time zone + 1h
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
//European Central Summer Time (CEST) check
bool BasicTime::isDST(time_t timestamp) {
	timestamp += _timezone * SECS_PER_HOUR;                   // time zone + 1h
	if (month(timestamp) >= 10 || month(timestamp) <= 3) {    // winter time/standard time from october to march
		if (month(timestamp) == 3 || month(timestamp) == 10) {
			int prevoiusSunday = day(timestamp) - weekday(timestamp);
			if (prevoiusSunday >= LAST_SUNDAY_OF_THE_MONTH) {
				if (weekday(timestamp) == 1) {    // last sunday of month
					if (hour(timestamp) < 2) {    // until 2:00 (2:00->3:00 in march and 3:00->2:00 in october)
						return month(timestamp) == 3 ? false : true;
					}
				}
				return month(timestamp) == 3 ? true : false;
			}
			return month(timestamp) == 3 ? false : false;
		}
		return false;
	}
	return true;
}
