#pragma once
#include <ESP8266WiFi.h>

class TCPService {
public:
    static void begin();
    static void update();
    static void disconnect();
    static bool isConnected();
    static uint16_t port() { return SERVER_PORT; }
    static bool isServerRunning() { return servidorLigado; }
    static bool hasNewMsg() { return newMsg; }
    static String getLastReceived();

private:
    static String normalizeText(const String& input);

    static constexpr uint16_t SERVER_PORT = 8080;
    inline static WiFiServer serverTCP{SERVER_PORT};
    inline static WiFiClient activeClient;
    inline static bool servidorLigado = false;
    inline static String lastReceived = "";
    inline static bool newMsg = false;
};