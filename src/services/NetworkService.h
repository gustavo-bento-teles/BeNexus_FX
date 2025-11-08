#pragma once
#include <ESP8266WiFi.h>

class NetworkService {
public:
    static void begin();
    static void update();
    static void connect(const char* ssid, const char* password);
    static void disconnect();
    static bool isConnected();

private:
    static bool tentandoConectar;
};