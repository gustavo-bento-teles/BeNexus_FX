#include "NetworkService.h"
#include <Arduino.h>

bool NetworkService::tentandoConectar = false;

void NetworkService::begin() {
    WiFi.mode(WIFI_OFF);
}

void NetworkService::update() {
    WiFiMode_t wifiMode = WiFi.getMode();

    if (WiFi.status() != WL_CONNECTED && wifiMode != WIFI_OFF && !tentandoConectar) {
        disconnect();
    }
}

void NetworkService::connect(const char* ssid, const char* password) {
    if (WiFi.status() == WL_CONNECTED) return;

    tentandoConectar = true;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(250);
    }

    tentandoConectar = false;
}

void NetworkService::disconnect() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

bool NetworkService::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}