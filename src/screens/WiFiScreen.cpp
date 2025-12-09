#include "screens/WiFiScreen.h"
#include "services/NetworkService.h"
#include <Arduino.h>

const char* ssid     = "Hotispoti";
const char* password = "#-&S3nha";

void WiFiScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
}

void WiFiScreen::update() {}

void WiFiScreen::draw() {
    display->clear();

    if (NetworkService::isConnected()) {
        String ip = WiFi.localIP().toString();
        int rssi = WiFi.RSSI();

        display->drawText(0, 10, "SSID:");
        display->drawText(40, 10, WiFi.SSID().c_str());

        display->drawText(0, 20, "IP:");
        display->drawText(40, 20, ip.c_str());

        char sinal[20];
        sprintf(sinal, "RSSI: %d dBm", rssi);
        display->drawText(0, 30, sinal);

        WiFiMode_t wifiMode = WiFi.getMode();
        const char* modeStr = "";
        switch (wifiMode) {
            case WIFI_OFF:     modeStr = "Radio WiFi OFF"; break;
            case WIFI_AP:      modeStr = "Radio WiFi AP"; break;
            case WIFI_STA:     modeStr = "Radio WiFi STA"; break;
            case WIFI_AP_STA:  modeStr = "Radio WiFi STA+AP"; break;
        }
        display->printCentered(modeStr, 45);

        display->printCentered("[Desligar WiFi]", 62);
    } 
    else {
        display->printCentered("Tela WiFi", 8);
        display->printCentered("[Ligar WiFi]", 38);

        WiFiMode_t wifiMode = WiFi.getMode();
        const char* modeStr = "";
        switch (wifiMode) {
            case WIFI_OFF:     modeStr = "Radio WiFi OFF"; break;
            case WIFI_AP:      modeStr = "Radio WiFi AP"; break;
            case WIFI_STA:     modeStr = "Radio WiFi STA"; break;
            case WIFI_AP_STA:  modeStr = "Radio WiFi STA+AP"; break;
        }
        display->printCentered(modeStr, 60);
    }

    display->display();
}

void WiFiScreen::end() {
    display->clear();
    display->display();
}

void WiFiScreen::onUpPressed() {
    nextTriggered = true;
}

void WiFiScreen::onDownPressed() {
    nextTriggered = true;
}

void WiFiScreen::onSelectPressed() {
    if (NetworkService::isConnected()) {
        NetworkService::disconnect();
    } else {
        display->clear();
        display->printCentered("Conectando em:", 30);
        display->printCentered(ssid, 45);
        display->display();

        NetworkService::connect(ssid, password);
    }
}