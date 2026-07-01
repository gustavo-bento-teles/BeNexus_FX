#pragma once
#include <DS3231.h>
#include <Wire.h>

class RTCManager {
public:
    void begin();
    void update();

    String getTimeString();
    String getDateString();

    void setTime(int hour, int minute, int second);
    void setDate(int day, int month, int year, int dow = 1);

    int getHour() const { return hour; }
    int getMinute() const { return minute; }
    int getSecond() const { return second; }

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
    int getDoW() const { return dow; }

    void setDateTime(struct tm* ti);

private:
    DS3231 rtc;
    bool century = false;
    bool h12Flag = false;
    bool pmFlag = false;

    int hour, minute, second;
    int day, month, year, dow;
};