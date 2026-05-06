#include "TimeServer.h"
#include <stdio.h>

const CityInfo TimeServer::CITIES[] = {
    {"Berlin", 1, true},
    {"New York", -5, true},
    {"Prague", 1, true},
    {"Doha", 3, false},
};

const int TimeServer::NUM_CITIES = sizeof(CITIES) / sizeof(CITIES[0]);

TimeServer::TimeServer() {
    handlers = {
        { Protocols::GET_TIME, [this](char* b){handleGetTime(b); return true;}},
        { Protocols::GET_TIME_WITHOUT_DATE, [this](char* b){handleGetTimeWithoutDate(b); return true;}},
        { Protocols::GET_TIME_SINCE_EPOCH, [this](char* b){handleGetTimeSinceEpoch(b); return true;}},
        { Protocols::GET_CLIENT_TO_SERVER_DELAY, [this](char* b){handleGetClientToServerDelay(b); return true;}},
        { Protocols::MEASURE_RTT, [this](char* b){handleMeasureRTT(b); return true;}},
        { Protocols::GET_TIME_WITHOUT_DATE_OR_SECONDS, [this](char* b){handleGetTimeWithoutDateOrSeconds(b); return true;}},
        { Protocols::GET_YEAR, [this](char* b){handleGetYear(b); return true;}},
        { Protocols::GET_MONTH_AND_DAY, [this](char* b){handleGetMonthAndDay(b); return true;}},
        { Protocols::GET_SECONDS_SINCE_MONTH_START, [this](char* b){handleGetSecondsSinceMonthStart(b); return true;}},
        { Protocols::GET_WEEK_OF_YEAR, [this](char* b){handleGetWeekOfYear(b); return true;}},
        { Protocols::GET_DAYLIGHT_SAVINGS, [this](char* b){handleGetDaylightSavings(b); return true;}},
        { Protocols::GET_TIME_IN_CITY, [this](char* b){handleGetTimeInCity(b); return true;}},
        { Protocols::MEASURE_TIME_LAP, [this](char* b){return handleMeasureTimeLap(b);}},
    };
}

bool TimeServer::handleRequest(const char* request, char* sendBuff) {
    auto it = handlers.find(request);
    if (it != handlers.end())
        return it->second(sendBuff);
    else
    {
        // If the request is not found in the handlers map, the following method checks if it's a city name.
        handleGetTimeInCityResponse(request, sendBuff);
        return true;
    }
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

// Handles the first part of GetTimeWithoutDateInCity request.
// Returns the list of cities for the client to choose from.
void TimeServer::handleGetTimeInCity(char* sendBuff) {
    strcpy(sendBuff, "Available cities:\n");
    for (int i = 0; i < NUM_CITIES; i++)
    {
        strcat(sendBuff, "  ");
        strcat(sendBuff, CITIES[i].name);
        strcat(sendBuff, "\n");
    }
    strcat(sendBuff, "Enter city name:");
}

// Handles the second part of GetTimeWithoutDateInCity request.
// Returns the time in the requested city. If the city is not found, returns the UTC time.
void TimeServer::handleGetTimeInCityResponse(const char* cityName, char* sendBuff) {
    
    tm*  localT = getCurrentTime();
    bool isDST  = localT->tm_isdst > 0;

    // Find the city in the table.
    int offset = 0;
    bool found = false;
    for (int i = 0; i < NUM_CITIES; i++)
    {
        if (strcmp(cityName, CITIES[i].name) == 0)
        {
            offset = CITIES[i].UTC_offset;
            if (isDST && CITIES[i].DST) offset += 1;
            found  = true;
            break;
        }
    }

    // Calculate the time in the requested city by using the UTC offset.
    time_t timer;
    time(&timer);
    timer += offset * 3600;
    tm* t = gmtime(&timer);

    if (found)
        sprintf(sendBuff, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    else
        sprintf(sendBuff, "City not found. UTC time: %02d:%02d:%02d",
                t->tm_hour, t->tm_min, t->tm_sec);
}

// Handles the MeasureTimeLap request.
// First call: starts the timer and returns a confirmation message.
// Second call: a) if less than 3 minutes have passed, returns the time passed.
//              b) if more than 3 minutes have passed, resets the timer silently.
bool TimeServer::handleMeasureTimeLap(char* sendBuff) {
    if (!lapActive)
    {
        // First call - start measuring
        lapStart  = GetTickCount();
        lapActive = true;
        sprintf(sendBuff, "Timer started. Send MeasureTimeLap again to get elapsed time.");
        return true;
    }
    else
    {
        DWORD now     = GetTickCount();
        DWORD elapsed = now - lapStart;

        if (elapsed > 180000)
        {
            // More than 3 minutes passed - reset timer
            lapActive = false;
            lapStart  = 0;
            sprintf(sendBuff, "Timer expired (over 3 minutes). Timer has been reset.");
            return false;
        }
        else
        {
            // Return the time passed and reset
            lapActive = false;
            lapStart  = 0;
            sprintf(sendBuff, "Elapsed time: %lu ms", elapsed);
            return true;
        }
    }
}