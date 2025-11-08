#pragma once
#include "screens/Screen.h"
#include "input/ButtonEvent.h"
#include "drivers/Display.h"

class ScreenManager {
public:
    explicit ScreenManager(Screen* initialScreen, Display* display);

    void begin();
    void update();
    void draw();
    void handleInput(ButtonEvent ev);

private:
    Screen* currentScreen;
    Display* display;
};