#include "WiFiScreen.hpp"
#include "services/NetworkService.hpp"
#include <Arduino.h>
#include <cstdio>

const char *ssid = "Hotispoti";
const char *password = "deauther";

void WiFiScreen::begin() {
  nextTriggered = false;
  driverContext.display->fontSet(u8g2_font_6x10_tr);
}

void WiFiScreen::update() {}

void WiFiScreen::draw() {
  driverContext.display->clear();

  ConnectionState state = NetworkService::getState();

  switch (state) {
  case ConnectionState::CONNECTED: {
    screenState = WiFiScreenState::CONNECTED;

    String ip = WiFi.localIP().toString();
    int rssi = WiFi.RSSI();

    driverContext.display->drawText(0, 10, "SSID:");
    driverContext.display->drawText(40, 10, WiFi.SSID().c_str());

    driverContext.display->drawText(0, 20, "IP:");
    driverContext.display->drawText(40, 20, ip.c_str());

    char sinal[20];
    sprintf(sinal, "RSSI: %d dBm", rssi);
    driverContext.display->drawText(0, 30, sinal);

    driverContext.display->printCentered("[Desligar WiFi]", 62);
    break;
  }

  case ConnectionState::CONNECTING: {
    screenState = WiFiScreenState::CONNECTING;

    static int dots = 0;
    static unsigned long lastUpdate = 0;

    if (millis() - lastUpdate > 400) {
      dots = (dots + 1) % 4;
      lastUpdate = millis();
    }

    char loading[20];
    sprintf(loading, "Conectando%s",
            dots == 0   ? ""
            : dots == 1 ? "."
            : dots == 2 ? ".."
                        : "...");

    char stringConnection[128];
    snprintf(stringConnection, sizeof(stringConnection), "%s:%s", ssid,
             password);

    driverContext.display->printCentered("Conectando em:", 14);
    driverContext.display->printCentered(stringConnection, 25);
    driverContext.display->printCentered(loading, 38);
    driverContext.display->printCentered("[Cancelar]", 62);
    break;
  }

  case ConnectionState::FAILED:
    screenState = WiFiScreenState::FAILED;

    driverContext.display->printCentered("Falha ao conectar", 20);
    driverContext.display->printCentered("[Tentar novamente]", 32);
    break;

  case ConnectionState::IDLE:
  default:
    screenState = WiFiScreenState::IDLE;

    driverContext.display->printCentered("Tela WiFi", 8);
    driverContext.display->printCentered("[Ligar WiFi]", 38);
    break;
  }

  WiFiMode_t wifiMode = WiFi.getMode();
  const char *modeStr = "";
  switch (wifiMode) {
  case WIFI_OFF:
    modeStr = "Radio WiFi OFF";
    break;
  case WIFI_AP:
    modeStr = "Radio WiFi AP";
    break;
  case WIFI_STA:
    modeStr = "Radio WiFi STA";
    break;
  case WIFI_AP_STA:
    modeStr = "Radio WiFi STA+AP";
    break;
  }
  driverContext.display->printCentered(modeStr, 54);

  driverContext.display->display();
}

void WiFiScreen::end() {
  driverContext.display->clear();
  driverContext.display->display();
}

void WiFiScreen::onUpPressed() { nextTriggered = true; }

void WiFiScreen::onDownPressed() { nextTriggered = true; }

void WiFiScreen::onSelectPressed() {
  ConnectionState state = NetworkService::getState();

  switch (state) {
  case ConnectionState::IDLE:
  case ConnectionState::FAILED:
    NetworkService::connect(ssid, password);
    break;

  case ConnectionState::CONNECTING:
    NetworkService::disconnect();
    break;

  case ConnectionState::CONNECTED:
    NetworkService::disconnect();
    break;
  }
}
