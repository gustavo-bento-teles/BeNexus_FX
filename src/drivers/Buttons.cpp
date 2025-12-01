#include "Buttons.h"
#include <Arduino.h>

Buttons::Buttons(uint8_t up, uint8_t down, uint8_t select)
    : upPin(up), downPin(down), selectPin(select) {}

void Buttons::begin() {
    pinMode(upPin, INPUT_PULLUP);
    pinMode(downPin, INPUT_PULLUP);
    pinMode(selectPin, INPUT_PULLUP);

    btnUp.attach(upPin);
    btnDown.attach(downPin);
    btnSelect.attach(selectPin);

    btnUp.interval(25);
    btnDown.interval(25);
    btnSelect.interval(25);
}

void Buttons::update() {
    btnUp.update();
    btnDown.update();
    btnSelect.update();

    if (btnUp.fell()) {
        upPressedAt = millis();
        upHoldFired = false;
    }

    if (btnDown.fell()) {
        downPressedAt = millis();
        downHoldFired = false;
    }

    if (btnSelect.fell()) {
        selectPressedAt = millis();
        selectHoldFired = false;
    }
}

bool Buttons::upPressed() {
    return btnUp.fell() && !upHoldFired;
}

bool Buttons::downPressed() {
    return btnDown.fell() && !downHoldFired;
}

bool Buttons::selectPressed() {
    return btnSelect.fell() && !selectHoldFired;
}

bool Buttons::upHeld(uint16_t holdMs) {
    if (!upHoldFired &&
        btnUp.read() == LOW &&
        millis() - upPressedAt >= holdMs) {

        upHoldFired = true;
        return true;
    }
    return false;
}


bool Buttons::downHeld(uint16_t holdMs) {
    if (!downHoldFired &&
        btnDown.read() == LOW &&
        millis() - downPressedAt >= holdMs) {

        downHoldFired = true;
        return true;
    }
    return false;
}


bool Buttons::selectHeld(uint16_t holdMs) {
    if (!selectHoldFired&&
        btnSelect.read() == LOW &&
        millis() - selectPressedAt >= holdMs) {

        selectHoldFired = true;
        return true;
    }
    return false;
}