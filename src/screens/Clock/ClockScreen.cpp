#include "ClockScreen.h"

void ClockScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
}

void ClockScreen::update() {
    rtcManager->update();
}

void ClockScreen::draw() {
    display->clear();

    display->fontSet(u8g2_font_9x15_mf);
    display->printCentered(rtcManager->getTimeString().c_str(), 22);

    display->fontSet(u8g2_font_6x10_tr);
    display->printCentered(rtcManager->getDateString().c_str(), 42);

    const char* dias[7] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};
    char dowStr[4];
    int dow = rtcManager->getDoW() - 1;
    sprintf(dowStr, "%s", dias[dow]);

    display->printCentered(dowStr, 52);

    display->display();
}

void ClockScreen::end() {
    display->clear();
    display->display();
}

void ClockScreen::onUpPressed() {
    nextTriggered = true;
}

void ClockScreen::onDownPressed() {
    nextTriggered = true;
}

void ClockScreen::onSelectPressed() {
    display->displayOn(false);
}