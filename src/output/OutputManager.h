#pragma once
#include "drivers/Lantern.h"

class OutputManager {
public:
    explicit OutputManager(Lantern& lantern);
    void begin();
    void update();

    void setLantern(bool on);
    void toggleLantern();
    bool isLanternOn() const;

private:
    Lantern& lantern;
};
