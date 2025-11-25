#include "CalendarScreen.h"

void CalendarScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
}

void CalendarScreen::update() {
    rtcManager->update();
}

void CalendarScreen::draw() {
    display->clear();

    int month = rtcManager->getMonth();
    int year = rtcManager->getYear();

    char header[32];
    snprintf(header, sizeof(header), "%02d/%04d", month, year);
    display->printCentered(header, 0);

    const char* weekDays[] = {"D", "S", "T", "Q", "Q", "S", "S"};
    for (int i = 0; i < 7; i++) { 
        display->drawText(7 + i * 17, 7 , weekDays[i]);
    }

    // Cálculo do primeiro dia do mês
    int firstDow = weekdayOf(1, month, year);
    int totalDays = daysInMonth(month, year);

    // Posição inicial
    int x0 = 4;
    int y0 = 14 ;
    int cellW = 17;
    int cellH = 10;

    int x = x0 + firstDow * cellW;
    int y = y0;

    for (int d = 1; d <= totalDays; d++) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%2d", d);
        display->drawText(x, y, buf);

        x += cellW;
        if ((firstDow + d) % 7 == 0) {
            x = x0;
            y += cellH;
        }
    }

    display->display();
}

void CalendarScreen::end() {
    display->clear();
    display->display();
}

void CalendarScreen::onUpPressed() {
    nextTriggered = true;
}

void CalendarScreen::onDownPressed() {
    nextTriggered = true;
}

void CalendarScreen::onSelectPressed() {
    nextTriggered = true;
}


int CalendarScreen::daysInMonth(int month, int year) {
    if (month == 2) return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
    return 31;
}


int CalendarScreen::weekdayOf(int d, int m, int y) {
    if (m < 3) { m += 12; y--; }
    int k = y % 100;
    int j = y / 100;
    int h = (d + 13*(m+1)/5 + k + k/4 + j/4 + 5*j) % 7;
    int weekday = (h + 6) % 7;
    return weekday;
}