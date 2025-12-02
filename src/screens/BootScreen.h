#pragma once
#include "Screen.h"
#include "drivers/Display.h"

#include "animations/Animator.h"
#include "animations/LineGrowAnimation.h"

class BootScreen : public Screen {
public:
    BootScreen(Display* disp, Screen* next);

    const char* name() override { return "BootScreen"; }

    void begin() override;
    void update() override;
    void draw() override;
    void end() override;
    Screen* nextScreen() override;

private:
    Animator animator;

    Display* display;
    Screen* nextScreenPtr;
    bool finished = false;
    unsigned long startTime = 0;
    const unsigned long bootDuration = 1000;
};