#include "CalendarScreen.hpp"

void CalendarScreen::begin() {
  nextTriggered = false;
  display->fontSet(u8g2_font_6x10_tf);

  currentYear = rtcManager->getYear();
  currentMonth = rtcManager->getMonth();
  currentDay = rtcManager->getDay();

  month = currentMonth;
  year = currentYear;
}

void CalendarScreen::update() {}

void CalendarScreen::draw() {
  display->clear();

  char header[32];

  if (month != 0 && year != 0) {
    const char *nameWeekDays[] = {
        "Janeiro", "Fevereiro", "Marco",    "Abril",   "Maio",     "Junho",
        "Julho",   "Agosto",    "Setembro", "Outubro", "Novembro", "Dezembro"};
    snprintf(header, sizeof(header), "< %02d(%s)/%04d >", month,
             nameWeekDays[month - 1], year + 2000);
    display->printCentered(header, 8);
  } else {
    snprintf(header, sizeof(header), "< %02d/%04d >", month, year + 2000);
    display->printCentered(header, 8);
  }

  const char *weekDays[] = {"D", "S", "T", "Q", "Q", "S", "S"};
  for (int i = 0; i < 7; i++) {
    display->drawText(7 + i * 17, 18, weekDays[i]);
  }

  int firstDow = weekdayOf(1, month, year);
  int totalDays = daysInMonth(month, year);

  int x0 = 4;
  int y0 = 28;
  int cellW = 18;
  int cellH = 7;

  int x = x0 + firstDow * cellW;
  int y = y0;

  for (int d = 1; d <= totalDays; d++) {
    char buf[4];
    if (d == currentDay && month == currentMonth && year == currentYear) {
      snprintf(buf, sizeof(buf), "%2d", d);
      int largura = display->getWStr(buf);

      display->drawColorSet(1);
      display->drawBox(x - 1, y - 7, largura + 3, 7);

      display->drawColorSet(0);
      display->drawText(x, y, buf);

      display->drawColorSet(1);
    } else {
      snprintf(buf, sizeof(buf), "%2d", d);
      display->drawText(x, y, buf);
    }

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
  month++;
  if (month > 12) {
    month = 1;
    year++;
  }
}

void CalendarScreen::onDownPressed() {
  month--;
  if (month <= 0) {
    month = 12;
    year--;
  }
}

void CalendarScreen::onSelectPressed() { nextTriggered = true; }

void CalendarScreen::onUpHeld() { year++; }

void CalendarScreen::onDownHeld() { year--; }

void CalendarScreen::onSelectHeld() {
  month = currentMonth;
  year = currentYear;
}

int CalendarScreen::daysInMonth(int month, int year) {
  if (month == 2)
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
  if (month == 4 || month == 6 || month == 9 || month == 11)
    return 30;
  return 31;
}

int CalendarScreen::weekdayOf(int d, int m, int y) {
  if (m < 3) {
    m += 12;
    y--;
  }
  int k = y % 100;
  int j = y / 100;
  int h = (d + 13 * (m + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
  int weekday = (h + 6) % 7;
  return weekday;
}
