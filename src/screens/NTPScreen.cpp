#include "screens/NTPScreen.h"
#include "services/NetworkService.h"
#include <Arduino.h>

void NTPScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);

    animator.add(new LineGrowAnimation(0, 11, 128, 5, true, false));
}

void NTPScreen::update() {
    animator.update();
}

void NTPScreen::draw() {
    display->clear();

    animator.draw(display);

    display->printCentered("Tela NTP", 8);

    if (NetworkService::isConnected()) {
        display->printCentered("[Atualizar NTP]", 38);
    } else {
        display->printCentered("Sem WiFi disponivel!", 30);
        display->printCentered("Use [tela WiFi]", 45);
        display->printCentered("para conectar", 55);
    }

    display->display();
}

void NTPScreen::end() {
    display->clear();
    display->display();
    animator.clear();
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
    } else {
        nextScreenPtr = wifiScreen;
        nextTriggered = true;
    }
}

void NTPScreen::atualizarNTP() {
    display->clear();
    display->printCentered("Atualizando NTP...", 32);
    display->printCentered("Aguarde...", 45);
    display->display();

    if (firstClient) {
        timeClient.begin();
        firstClient = false;
    }

    bool ok = timeClient.forceUpdate();
    display->clear();

    if (!ok) {
        display->printCentered("Atualizacao falhou!", 32);
        display->display();
        delay(1000);
        return;
    }

    display->printCentered("NTP atualizado!", 32);
    display->display();
    delay(1000);

    time_t rawtime = timeClient.getEpochTime();
    struct tm *ti = localtime(&rawtime);
    rtcManager->setDateTime(ti);

    display->clear();
    display->printCentered("RTC atualizado!", 32);
    display->display();
    delay(1000);
}