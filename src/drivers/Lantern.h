#pragma once
#include <Arduino.h>

class Lantern {
public:
    Lantern(uint8_t pin);

    void begin();

    bool getLanternStatus();
    void setLanternStatus(bool status);
    void toggle();

private:
    uint8_t pin;

    bool lanternStatus = false;
};