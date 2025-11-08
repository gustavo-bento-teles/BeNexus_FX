#pragma once
#include "Screen.h"
#include "drivers/Display.h"
#include "output/OutputManager.h"

class FlashlightScreen : public Screen {
public:
    FlashlightScreen(Display* disp, Screen* next, OutputManager* outMgr)
        : display(disp), nextScreenPtr(next), output(outMgr), nextTriggered(false) {}

    const char* name() override { return "FlashlightScreen"; }

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
    OutputManager* output;
    bool nextTriggered;
};