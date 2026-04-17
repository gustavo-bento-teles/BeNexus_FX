#pragma once
#include "screens/Screen.h"
#include "drivers/Display.h"
#include "drivers/RTC.h"

enum class ClockScreenState {
    IDLE
};

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

    uint8_t getState() const override { return static_cast<int>(screenState); }

protected:
    void onSelectPressed() override;
    void onUpPressed() override;
    void onDownPressed() override;

private:
    Display* display;
    RTCManager* rtcManager;
    Screen* nextScreenPtr;
    bool nextTriggered;

    ClockScreenState screenState = ClockScreenState::IDLE;
};