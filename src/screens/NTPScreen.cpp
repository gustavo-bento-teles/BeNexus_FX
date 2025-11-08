#include "screens/NTPScreen.h"
#include "services/NetworkService.h"
#include <Arduino.h>

void NTPScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
}

void NTPScreen::update() {}

void NTPScreen::draw() {
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);

    display->printCentered("NTP Screen", 8);

    if (NetworkService::isConnected()) {
        display->printCentered("[Update NTP]", 38);
    } else {
        display->printCentered("No WiFi available!", 30);
        display->printCentered("Use [WiFi Screen]", 45);
        display->printCentered("to connect", 55);
    }

    display->display();
}

void NTPScreen::end() {
    display->clear();
    display->display();
}

void NTPScreen::onUpPressed() {
    nextTriggered = true;
}

void NTPScreen::onDownPressed() {
    nextTriggered = true;
}

void NTPScreen::onSelectPressed() {
    if (NetworkService::isConnected()) {
        atualizarNTP();
    }
}

void NTPScreen::atualizarNTP() {
    display->clear();
    display->printCentered("Updating NTP...", 32);
    display->printCentered("Wait...", 45);
    display->display();

    if (firstClient) {
        timeClient.begin();
        firstClient = false;
    }

    bool ok = timeClient.forceUpdate();
    display->clear();

    if (!ok) {
        display->printCentered("Update failed!", 32);
        display->display();
        delay(1000);
        return;
    }

    display->printCentered("NTP updated!", 32);
    display->display();
    delay(1000);

    time_t rawtime = timeClient.getEpochTime();
    struct tm *ti = localtime(&rawtime);
    rtcManager->setDateTime(ti);

    display->clear();
    display->printCentered("RTC updated!", 32);
    display->display();
    delay(1000);
}