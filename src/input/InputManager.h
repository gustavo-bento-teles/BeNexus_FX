#pragma once
#include "drivers/Buttons.h"
#include "ButtonEvent.h"

class InputManager {
public:
    explicit InputManager(Buttons& btns);

    void begin();
    void update();
    ButtonEvent getEvent();

private:
    Buttons& buttons;
};