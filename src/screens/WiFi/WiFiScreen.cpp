#include "WiFiScreen.hpp"
#include "services/NetworkService.hpp"
#include <Arduino.h>

const char *ssid = "Hotispoti";
const char *password = "#-&S3nha";

void WiFiScreen::begin() {
  nextTriggered = false;
  display->fontSet(u8g2_font_6x10_tr);
}

void WiFiScreen::update() {}

void WiFiScreen::draw() {
  display->clear();

  ConnectionState state = NetworkService::getState();

  switch (state) {
  case ConnectionState::CONNECTED: {
    screenState = WiFiScreenState::CONNECTED;

    String ip = WiFi.localIP().toString();
    int rssi = WiFi.RSSI();

    display->drawText(0, 10, "SSID:");
    display->drawText(40, 10, WiFi.SSID().c_str());

    display->drawText(0, 20, "IP:");
    display->drawText(40, 20, ip.c_str());

    char sinal[20];
    sprintf(sinal, "RSSI: %d dBm", rssi);
    display->drawText(0, 30, sinal);

    display->printCentered("[Desligar WiFi]", 62);
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

    display->printCentered("Conectando em:", 14);
    display->printCentered(ssid, 25);
    display->printCentered(loading, 38);
    display->printCentered("[Cancelar]", 62);
    break;
  }

  case ConnectionState::FAILED:
    screenState = WiFiScreenState::FAILED;

    display->printCentered("Falha ao conectar", 20);
    display->printCentered("[Tentar novamente]", 32);
    break;

  case ConnectionState::IDLE:
  default:
    screenState = WiFiScreenState::IDLE;

    display->printCentered("Tela WiFi", 8);
    display->printCentered("[Ligar WiFi]", 38);
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
  display->printCentered(modeStr, 54);

  display->display();
}

void WiFiScreen::end() {
  display->clear();
  display->display();
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
