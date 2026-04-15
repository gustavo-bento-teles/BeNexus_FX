#pragma once
#include "screens/Screen.h"
#include "input/ButtonEvent.h"
#include "drivers/Display.h"
#include "drivers/RTC.h"

class ScreenManager {
public:
    explicit ScreenManager(Screen* initialScreen, Display* display, RTCManager* rtc);

    void begin();
    void update();
    void draw();
    void handleInput(ButtonEvent ev);

private:
    Screen* currentScreen;
    Display* display;
    RTCManager* rtc;

    uint8_t currentStateScreen = -1;
};