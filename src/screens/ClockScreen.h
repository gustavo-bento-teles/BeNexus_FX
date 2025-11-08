#pragma once
#include "Screen.h"
#include "drivers/Display.h"
#include "drivers/RTC.h"

class ClockScreen : public Screen {
public:
    ClockScreen(Display* disp, RTCManager* rtc, Screen* next)
        : display(disp), rtcManager(rtc), nextScreenPtr(next), nextTriggered(false) {}
    
    const char* name() override { return "ClcokScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;

    Screen* nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

protected:
    void onSelectPressed() override;
    void onUpPressed() override;
    void onDownPressed() override;

private:
    Display* display;
    RTCManager* rtcManager;
    Screen* nextScreenPtr;
    bool nextTriggered;
};