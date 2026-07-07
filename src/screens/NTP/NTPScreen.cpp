#include "NTPScreen.hpp"
#include "core/StaticRegistry.hpp"
#include "services/NetworkService.hpp"
#include <Arduino.h>

void NTPScreen::begin() {
  nextTriggered = false;
  driverContext.display->fontSet(u8g2_font_6x10_tr);
}

void NTPScreen::update() {}

void NTPScreen::draw() {
  driverContext.display->clear();

  driverContext.display->printCentered("Tela NTP", 8);

  if (NetworkService::isConnected()) {
    screenState = NTPScreenState::CONNECTED;

    driverContext.display->printCentered("[Atualizar NTP]", 38);
  } else {
    screenState = NTPScreenState::DISCONNECTED;

    driverContext.display->printCentered("Sem WiFi disponivel!", 30);
    driverContext.display->printCentered("Use [tela WiFi]", 45);
    driverContext.display->printCentered("para conectar", 55);
  }

  driverContext.display->display();
}

void NTPScreen::end() {
  driverContext.display->clear();
  driverContext.display->display();
}

void NTPScreen::onUpPressed() { nextTriggered = true; }

void NTPScreen::onDownPressed() { nextTriggered = true; }

void NTPScreen::onSelectPressed() {
  if (NetworkService::isConnected()) {
    atualizarNTP();
  } else {
    nextScreenID = ScreenID::WIFI;
    nextTriggered = true;
  }
}

void NTPScreen::atualizarNTP() {
  screenState = NTPScreenState::UPDATING;

  driverContext.display->clear();
  driverContext.display->printCentered("Atualizando NTP...", 32);
  driverContext.display->printCentered("Aguarde...", 45);
  driverContext.display->display();

  if (firstClient) {
    timeClient.begin();
    firstClient = false;
  }

  bool ok = timeClient.forceUpdate();
  driverContext.display->clear();

  if (!ok) {
    driverContext.display->printCentered("Atualizacao falhou!", 32);
    driverContext.display->display();
    delay(1000);
    return;
  }

  driverContext.display->printCentered("NTP atualizado!", 32);
  driverContext.display->display();
  delay(1000);

  time_t rawtime = timeClient.getEpochTime();
  struct tm *ti = localtime(&rawtime);
  driverContext.rtc->setDateTime(ti);

  driverContext.display->clear();
  driverContext.display->printCentered("RTC atualizado!", 32);
  driverContext.display->display();
  delay(1000);
}
