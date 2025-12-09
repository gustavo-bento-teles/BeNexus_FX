#pragma once
#include "Screen.h"
#include "drivers/Display.h"
#include <ESP8266WiFi.h>

#include "animations/Animator.h"
#include "animations/LineGrowAnimation.h"

class WiFiScreen : public Screen {
public:
    WiFiScreen(Display* disp, Screen* next)
        : display(disp), nextScreenPtr(next), nextTriggered(false) {}

    const char* name() override { return "WiFiScreen"; }

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
};