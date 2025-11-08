#include "drivers/RTC.h"

void RTCManager::begin() {
    Wire.begin();
    rtc.setClockMode(false);
}

void RTCManager::update() {
    second = rtc.getSecond();
    minute = rtc.getMinute();
    hour   = rtc.getHour(h12Flag, pmFlag);

    day    = rtc.getDate();
    month  = rtc.getMonth(century);
    year   = rtc.getYear();
    dow    = rtc.getDoW();
}

String RTCManager::getTimeString() {
    char buf[9];
    sprintf(buf, "%02d:%02d:%02d", hour, minute, second);
    return String(buf);
}

String RTCManager::getDateString() {
    char buf[11];
    sprintf(buf, "%02d/%02d/20%02d", day, month, year);
    return String(buf);
}

void RTCManager::setTime(int h, int m, int s) {
    rtc.setHour(h);
    rtc.setMinute(m);
    rtc.setSecond(s);
}

void RTCManager::setDate(int d, int mo, int y, int dow) {
    rtc.setDate(d);
    rtc.setMonth(mo);
    rtc.setYear(y - 2000);
    rtc.setDoW(dow);
}

void RTCManager::setDateTime(struct tm* ti) {
    rtc.setYear(ti->tm_year + 1900 - 2000);
    rtc.setMonth(ti->tm_mon + 1);
    rtc.setDate(ti->tm_mday);
    rtc.setDoW((ti->tm_wday == 0) ? 1 : ti->tm_wday + 1);
    rtc.setHour(ti->tm_hour);
    rtc.setMinute(ti->tm_min);
    rtc.setSecond(ti->tm_sec);
}
