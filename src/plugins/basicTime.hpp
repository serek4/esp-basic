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

// time_t requestNtpTime();
// void NTPrequestCallback(AsyncUDPPacket &packet);
// bool sendNTPpacket(IPAddress &address);
// void NTPsyncInterval(const char * message);

class BasicTime {
  public:
	void setup();
    void waitForNTP(int waitTime = 10);
	void handle();
	String parseTimestamp(time_t timeStamp, bool wholeDate = true);

	BasicTime();

  private:
	bool _inclTime;
	static time_t _requestNtpTime();
	static void _NTPrequestCallback(AsyncUDPPacket &packet);
	static bool _sendNTPpacket(IPAddress &address);
	static void _NTPsyncInterval(const char *message);

	friend class BasicSetup;
};

extern BasicTime _basicTime;
