#pragma once
#include "input/InputManager.h"

class Screen {
public:
    virtual ~Screen() {}

    virtual const char* name() { return "Unnamed"; }

    virtual void begin() {};
    virtual void end() {}
    virtual void update() {}
    virtual void draw() = 0;
    virtual void handleInput(ButtonEvent ev);
    virtual Screen* nextScreen() { return nullptr; }

protected:
    virtual void onUpPressed() {}
    virtual void onDownPressed() {}
    virtual void onSelectPressed() {}

    virtual void onUpHeld() {}
    virtual void onDownHeld() {}
    virtual void onSelectHeld() {}
};