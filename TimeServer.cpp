#include "TimeServer.h"
#include <stdio.h>

TimeServer::TimeServer() {
    handlers = {
        { Protocols::GET_TIME, [this](char* b) { handleGetTime(b); } },
        { Protocols::GET_TIME_WITHOUT_DATE, [this](char* b) { handleGetTimeWithoutDate(b); } },
        { Protocols::GET_TIME_SINCE_EPOCH, [this](char* b) { handleGetTimeSinceEpoch(b); } },
        { Protocols::GET_CLIENT_TO_SERVER_DELAY, [this](char* b) { handleGetClientToServerDelay(b); } },
        { Protocols::MEASURE_RTT, [this](char* b) { handleMeasureRTT(b); } },
        { Protocols::GET_TIME_WITHOUT_DATE_OR_SECONDS, [this](char* b) { handleGetTimeWithoutDateOrSeconds(b); } },
        { Protocols::GET_YEAR, [this](char* b) { handleGetYear(b); } },
        { Protocols::GET_MONTH_AND_DAY, [this](char* b) { handleGetMonthAndDay(b); } },
        { Protocols::GET_SECONDS_SINCE_MONTH_START, [this](char* b) { handleGetSecondsSinceMonthStart(b); } },
        { Protocols::GET_WEEK_OF_YEAR, [this](char* b) { handleGetWeekOfYear(b); } },
        { Protocols::GET_DAYLIGHT_SAVINGS, [this](char* b) { handleGetDaylightSavings(b); } },
        { Protocols::GET_TIME_IN_CITY, [this](char* b) { handleGetTimeInCity(b); } },
        { Protocols::MEASURE_TIME_LAP, [this](char* b) { handleMeasureTimeLap(b); } },
    };
}

void TimeServer::handleRequest(const char* request, char* sendBuff) {
    auto it = handlers.find(request);
    if (it != handlers.end())
        it->second(sendBuff);
    else
        sprintf(sendBuff, "ERROR: Unknown request");
}

// Returns a pointer to a tm struct representing the current local time.
tm* TimeServer::getCurrentTime() {
    time_t timer;
    time(&timer);
    return localtime(&timer);
}

// Returns the current date and time in the format: dd/mm/yyyy hh:mm:ss
void TimeServer::handleGetTime(char* sendBuff) {
    tm* t = getCurrentTime();
    sprintf(sendBuff, "%02d/%02d/%04d %02d:%02d:%02d",
        t->tm_mday,
        t->tm_mon  + 1,
        t->tm_year + 1900,
        t->tm_hour,
        t->tm_min,
        t->tm_sec);
}

// Returns the current time (without date) in the format: hh:mm:ss
void TimeServer::handleGetTimeWithoutDate(char* sendBuff) {
    tm* t = getCurrentTime();
    sprintf(sendBuff, "%02d:%02d:%02d",
        t->tm_hour,
        t->tm_min,
        t->tm_sec);
}

// Returns the number of seconds elapsed since the Unix Epoch (01/01/1970).
void TimeServer::handleGetTimeSinceEpoch(char* sendBuff) {
    time_t timer;
    time(&timer);
    sprintf(sendBuff, "%lld", (long long)timer);
}

// Returns the current server timestamp in milliseconds using GetTickCount().
// Used by the client to estimate one-way client-to-server delay.
void TimeServer::handleGetClientToServerDelay(char* sendBuff) {
    sprintf(sendBuff, "%lu", GetTickCount());
}

// Returns a simple acknowledgment used by the client to measure RTT.
void TimeServer::handleMeasureRTT(char* sendBuff) {
    sprintf(sendBuff, "OK");
}

// Returns the current time (without date or seconds) in the format: hh:mm
void TimeServer::handleGetTimeWithoutDateOrSeconds(char* sendBuff) {
    tm* t = getCurrentTime();
    sprintf(sendBuff, "%02d:%02d",
        t->tm_hour,
        t->tm_min);
}

// Returns the current year (e.g. 2026).
void TimeServer::handleGetYear(char* sendBuff) {
    tm* t = getCurrentTime();
    sprintf(sendBuff, "%04d", t->tm_year + 1900);
}

// Returns the current month and day in the format: mm/dd
void TimeServer::handleGetMonthAndDay(char* sendBuff) {
    tm* t = getCurrentTime();
    sprintf(sendBuff, "%02d/%02d",
        t->tm_mon + 1,
        t->tm_mday);
}

// Returns the number of seconds elapsed since the beginning of the current month.
void TimeServer::handleGetSecondsSinceMonthStart(char* sendBuff) {
    tm* t = getCurrentTime();
    long long seconds = (long long)(t->tm_mday - 1) * 86400
                      + t->tm_hour * 3600
                      + t->tm_min  * 60
                      + t->tm_sec;
    sprintf(sendBuff, "%lld", seconds);
}

// Returns the current week number since the beginning of the year (starting at 1).
void TimeServer::handleGetWeekOfYear(char* sendBuff) {
    tm* t = getCurrentTime();
    sprintf(sendBuff, "%d", t->tm_yday / 7 + 1);
}

// Returns 1 if Daylight Saving Time is currently in effect, 0 otherwise.
void TimeServer::handleGetDaylightSavings(char* sendBuff) {
    tm* t = getCurrentTime();
    sprintf(sendBuff, "%d", t->tm_isdst > 0 ? 1 : 0);
}

// To be implemented in Step 5.
void TimeServer::handleGetTimeInCity(char* sendBuff) {
    sprintf(sendBuff, "TODO");
}

// To be implemented in Step 8.
void TimeServer::handleMeasureTimeLap(char* sendBuff) {
    sprintf(sendBuff, "TODO");
}