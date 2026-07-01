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

    bool upHeld(uint16_t holdMs = 500);
    bool downHeld(uint16_t holdMs = 500);
    bool selectHeld(uint16_t holdMs = 500);

private:
    Bounce btnUp, btnDown, btnSelect;

    uint32_t upPressedAt = 0;
    uint32_t downPressedAt = 0;
    uint32_t selectPressedAt = 0;

    bool upHoldFired = false;
    bool downHoldFired = false;
    bool selectHoldFired = false;

    uint8_t upPin, downPin, selectPin;
};