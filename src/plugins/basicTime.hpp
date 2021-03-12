#pragma once

#include "../espBasicSetup.hpp"
#include <ESPAsyncUDP.h>
#include <TimeLib.h>

// clang-format off
// Setup debug printing macros.
#ifdef BASIC_TIME_DEBUG
#define BASICTIME_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASICTIME_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
#define BASICTIME_PRINT(...) {}
#define BASICTIME_PRINTLN(...) {}
#endif
// clang-format on

#define NTP_PACKET_SIZE 48    // NTP time stamp is in the first 48 bytes of the message
#define NTP_TIMEOUT 1500

class BasicTime {
  public:
	void setup();
	bool waitForNTP(int waitTime = 10);
	void handle();
	static String dateString(time_t timeStamp);
	static String timeString(time_t timeStamp);
	static String dateTimeString(time_t timeStamp);

	BasicTime(const char *NTP_server_address, int NTP_server_port, int timezone, bool summertime);
	~BasicTime();

  private:
	static char _NTP_server_address[32];
	static int _NTP_server_port;
	static int _timezone;
	static bool _summertime;
	static bool _waitingForNTP;
	static u_long _requestSendedAt;
	static bool _gotNTPserverIP;
	static IPAddress _timeServerIP;      // NTP server address
	static time_t _NTPSyncInterval;      // timeSet sync interval
	static time_t _NTPReSyncInterval;    // timeNeedsSync sync interval
	static time_t _NTPnoSyncInterval;    // timeNotSet sync interval
	static time_t _requestNtpTime();
	static void _NTPrequestCallback(AsyncUDPPacket &packet);
	static bool _sendNTPpacket(IPAddress &address);
	static void _NTPsyncInterval(const char *message);

	friend class BasicConfig;
};
