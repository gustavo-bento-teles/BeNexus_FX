#pragma once
#include <ESP8266WiFi.h>

enum class ConnectionState {
    IDLE,
    CONNECTING,
    CONNECTED,
    FAILED
};

class NetworkService {
public:
    static void begin();
    static void update();
    static void connect(const char* ssid, const char* password);
    static void disconnect(ConnectionState newState = ConnectionState::IDLE);
    static bool isConnected();
    static ConnectionState getState() { return state; }

private:
    static ConnectionState state;
    static unsigned long connectStartTime;
    static const unsigned long timeout = 10000;
};