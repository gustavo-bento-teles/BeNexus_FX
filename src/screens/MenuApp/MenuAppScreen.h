#pragma once
#include "screens/Screen.h"
#include "drivers/Display.h"

#include "screens/animations/Animator.h"
#include "screens/animations/LineGrowAnimation.h"

class MenuAppScreen : public Screen {
public:
    MenuAppScreen(Display* disp)
        : display(disp), selectedIndex(0), nextScreenPtr(nullptr), nextTriggered(false) {}

    const char* name() override { return "MenuAppScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;

    Screen* nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

    void setScreens(Screen* clk, Screen* calendar, Screen* wifi, Screen* tcp, Screen* ntp, Screen* flash, Screen* diagnostics) {
        clockScreen = clk;
        calendarScreen = calendar;
        wifiScreen = wifi;
        tcpScreen = tcp;
        ntpScreen = ntp;
        flashlightScreen = flash;
        diagnosticsScreen = diagnostics;
    }

protected:
    void onUpPressed() override;
    void onDownPressed() override;
    void onSelectPressed() override;

private:
    Screen* clockScreen;
    Screen* calendarScreen;
    Screen* wifiScreen;
    Screen* tcpScreen;
    Screen* ntpScreen;
    Screen* flashlightScreen;
    Screen* diagnosticsScreen;

    Display* display;
    Screen* nextScreenPtr;
    bool nextTriggered;

    Animator animator;

    int selectedIndex = 0;
    int offset = 0;
    const int numMaxVisible = 5;

    static constexpr const char* options[7] = {
        "Relogio/Data",
        "Calendario",
        "Tela WiFi",
        "Tela TCP",
        "Tela NTP",
        "Lanterna",
        "Diagnosticos"
    };

    static constexpr int numOptions = 7;
};