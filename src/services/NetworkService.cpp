#include "NetworkService.h"
#include <Arduino.h>

ConnectionState NetworkService::state = ConnectionState::IDLE;
unsigned long NetworkService::connectStartTime = 0;

void NetworkService::begin() {
    WiFi.mode(WIFI_OFF);
}

void NetworkService::update() {
    switch (state) {
        case ConnectionState::CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                state = ConnectionState::CONNECTED;
            } else if (millis() - connectStartTime > timeout) {
                disconnect(ConnectionState::FAILED);
            }
            break;

        case ConnectionState::CONNECTED:
            if (WiFi.status() != WL_CONNECTED) {
                state = ConnectionState::IDLE;
            }
            break;

        case ConnectionState::IDLE:
        case ConnectionState::FAILED:
            if (WiFi.status() != WL_CONNECTED && WiFi.getMode() != WIFI_OFF) {
                disconnect();
            }
            break;
    }
}

void NetworkService::connect(const char* ssid, const char* password) {
    if (state == ConnectionState::CONNECTING || WiFi.status() == WL_CONNECTED)
        return;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    state = ConnectionState::CONNECTING;
    connectStartTime = millis();
}

void NetworkService::disconnect(ConnectionState newState) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    state = newState;
}

bool NetworkService::isConnected() {
    return state == ConnectionState::CONNECTED;
}