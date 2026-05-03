#pragma once

namespace Protocols {
    constexpr const char* GET_TIME = "GetTime";
    constexpr const char* GET_TIME_WITHOUT_DATE = "GetTimeWithoutDate";
    constexpr const char* GET_TIME_SINCE_EPOCH = "GetTimeSinceEpoch";
    constexpr const char* GET_CLIENT_TO_SERVER_DELAY = "GetClientToServerDelayEstimation";
    constexpr const char* MEASURE_RTT = "MeasureRTT";
    constexpr const char* GET_TIME_WITHOUT_DATE_OR_SECONDS = "GetTimeWithoutDateOrSeconds";
    constexpr const char* GET_YEAR = "GetYear";
    constexpr const char* GET_MONTH_AND_DAY = "GetMonthAndDay";
    constexpr const char* GET_SECONDS_SINCE_MONTH_START = "GetSecondsSinceBeginingOfMonth";
    constexpr const char* GET_WEEK_OF_YEAR = "GetWeekOfYear";
    constexpr const char* GET_DAYLIGHT_SAVINGS = "GetDaylightSavings";
    constexpr const char* GET_TIME_IN_CITY = "GetTimeWithoutDateInCity";
    constexpr const char* MEASURE_TIME_LAP = "MeasureTimeLap";
}