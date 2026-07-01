#include "ClockScreen.hpp"

void ClockScreen::begin() { nextTriggered = false; }

void ClockScreen::update() {}

void ClockScreen::draw() {
  driverContext.display->clear();

  driverContext.display->fontSet(u8g2_font_9x15_mf);
  driverContext.display->printCentered(
      driverContext.rtc->getTimeString().c_str(), 22);

  driverContext.display->fontSet(u8g2_font_6x10_tr);
  driverContext.display->printCentered(
      driverContext.rtc->getDateString().c_str(), 42);

  const char *dias[7] = {"Domingo", "Segunda", "Terca", "Quarta",
                         "Quinta",  "Sexta",   "Sabado"};
  char dowStr[4];
  int dow = driverContext.rtc->getDoW() - 1;
  sprintf(dowStr, "%s", dias[dow]);

  driverContext.display->printCentered(dowStr, 52);

  driverContext.display->display();
}

void ClockScreen::end() {
  driverContext.display->clear();
  driverContext.display->display();
}

void ClockScreen::onUpPressed() { nextTriggered = true; }

void ClockScreen::onDownPressed() { nextTriggered = true; }

void ClockScreen::onSelectPressed() { driverContext.display->displayOn(false); }
