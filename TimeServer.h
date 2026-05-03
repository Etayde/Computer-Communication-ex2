#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <unordered_map>
#include <functional>
#include <string>
#include "Protocols.h"
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <time.h>

using namespace std;
using Handler = function<void(char*)>;

class TimeServer {
public:
    TimeServer();
    void handleRequest(const char* request, char* sendBuff);

private:
    unordered_map<string, Handler> handlers;

    tm* getCurrentTime();

    void handleGetTime(char* sendBuff);
    void handleGetTimeWithoutDate(char* sendBuff);
    void handleGetTimeSinceEpoch(char* sendBuff);
    void handleGetClientToServerDelay(char* sendBuff);
    void handleMeasureRTT(char* sendBuff);
    void handleGetTimeWithoutDateOrSeconds(char* sendBuff);
    void handleGetYear(char* sendBuff);
    void handleGetMonthAndDay(char* sendBuff);
    void handleGetSecondsSinceMonthStart(char* sendBuff);
    void handleGetWeekOfYear(char* sendBuff);
    void handleGetDaylightSavings(char* sendBuff);
    void handleGetTimeInCity(char* sendBuff);
    void handleMeasureTimeLap(char* sendBuff);

    // state for step 8
    DWORD lapStart  = 0;
    bool  lapActive = false;
};