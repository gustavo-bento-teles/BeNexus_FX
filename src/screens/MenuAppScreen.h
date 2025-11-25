#pragma once
#include "Screen.h"
#include "drivers/Display.h"

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

    void setScreens(Screen* clk, Screen* calendar, Screen* wifi, Screen* ntp, Screen* flash) {
        clockScreen = clk;
        calendarScreen = calendar;
        wifiScreen = wifi;
        ntpScreen = ntp;
        flashlightScreen = flash;
    }

protected:
    void onUpPressed() override;
    void onDownPressed() override;
    void onSelectPressed() override;

private:
    Screen* clockScreen;
    Screen* calendarScreen;
    Screen* wifiScreen;
    Screen* ntpScreen;
    Screen* flashlightScreen;

    Display* display;
    int selectedIndex = 0;
    Screen* nextScreenPtr;
    bool nextTriggered;
    
    static constexpr const char* options[5] = {
        "Relogio/Data",
        "Calendario",
        "WiFi",
        "NTP",
        "Lanterna"
    };
    static constexpr int numOptions = 5;
};