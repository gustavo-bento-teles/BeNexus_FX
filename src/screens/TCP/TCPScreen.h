#pragma once
#include "screens/Screen.h"
#include "drivers/Display.h"
#include <ESP8266WiFi.h>

class TCPScreen : public Screen {
public:
    TCPScreen(Display* disp, Screen* next, Screen* wifi)
        : display(disp), nextScreenPtr(next), wifiScreen(wifi), menuAppScreen(next), nextTriggered(false) {}
    
    const char* name() override { return "TCPScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;

    Screen* nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

protected:
    void onUpPressed() override;
    void onDownPressed() override;
    void onSelectPressed() override;

private:
    Display* display;
    Screen* nextScreenPtr;
    bool nextTriggered;

    Screen* wifiScreen;
    Screen* menuAppScreen;

    static constexpr uint8_t numMaxVisible = 5;
    String tcpCmds[numMaxVisible] = {"", "", "", "", ""};
    uint8_t contador = 0;
};