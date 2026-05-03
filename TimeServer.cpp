#include "TimeServer.h"
#include <stdio.h>

TimeServer::TimeServer() {
    handlers = {
        { Protocol::GET_TIME, [this](char* b) { handleGetTime(b); } },
        { Protocol::GET_TIME_WITHOUT_DATE, [this](char* b) { handleGetTimeWithoutDate(b); } },
        { Protocol::GET_TIME_SINCE_EPOCH, [this](char* b) { handleGetTimeSinceEpoch(b); } },
        { Protocol::GET_CLIENT_TO_SERVER_DELAY, [this](char* b) { handleGetClientToServerDelay(b); } },
        { Protocol::MEASURE_RTT, [this](char* b) { handleMeasureRTT(b); } },
        { Protocol::GET_TIME_WITHOUT_DATE_OR_SECONDS, [this](char* b) { handleGetTimeWithoutDateOrSeconds(b); } },
        { Protocol::GET_YEAR, [this](char* b) { handleGetYear(b); } },
        { Protocol::GET_MONTH_AND_DAY, [this](char* b) { handleGetMonthAndDay(b); } },
        { Protocol::GET_SECONDS_SINCE_MONTH_START, [this](char* b) { handleGetSecondsSinceMonthStart(b); } },
        { Protocol::GET_WEEK_OF_YEAR, [this](char* b) { handleGetWeekOfYear(b); } },
        { Protocol::GET_DAYLIGHT_SAVINGS, [this](char* b) { handleGetDaylightSavings(b); } },
        { Protocol::GET_TIME_IN_CITY, [this](char* b) { handleGetTimeInCity(b); } },
        { Protocol::MEASURE_TIME_LAP, [this](char* b) { handleMeasureTimeLap(b); } },
    };
}

void TimeServer::handleRequest(const char* request, char* sendBuff) {
    auto it = handlers.find(request);
    if (it != handlers.end())
        it->second(sendBuff);
    else
        sprintf(sendBuff, "ERROR: Unknown request");
}

tm* TimeServer::getCurrentTime() {
    time_t timer;
    time(&timer);
    return localtime(&timer);
}

// placeholders
void TimeServer::handleGetTime(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetTimeWithoutDate(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetTimeSinceEpoch(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetClientToServerDelay(char* sendBuff) { sprintf(sendBuff, "%lu", GetTickCount()); }
void TimeServer::handleMeasureRTT(char* sendBuff) { sprintf(sendBuff, "OK"); }
void TimeServer::handleGetTimeWithoutDateOrSeconds(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetYear(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetMonthAndDay(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetSecondsSinceMonthStart(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetWeekOfYear(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetDaylightSavings(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleGetTimeInCity(char* sendBuff) { sprintf(sendBuff, "TODO"); }
void TimeServer::handleMeasureTimeLap(char* sendBuff) { sprintf(sendBuff, "TODO"); }