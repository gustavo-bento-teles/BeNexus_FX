#pragma once
#include <Bounce2.h>

class Buttons {
public:
    Buttons(uint8_t upPin, uint8_t downPin, uint8_t selectPin);
    void begin();
    void update();

    bool upPressed();
    bool downPressed();
    bool selectPressed();

private:
    Bounce btnUp, btnDown, btnSelect;
    uint8_t upPin, downPin, selectPin;
};