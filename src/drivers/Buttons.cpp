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
}

bool Buttons::upPressed() {
    return btnUp.fell();
}

bool Buttons::downPressed() {
    return btnDown.fell();
}

bool Buttons::selectPressed() {
    return btnSelect.fell();
}