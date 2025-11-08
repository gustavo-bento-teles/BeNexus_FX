#include "Lantern.h"

Lantern::Lantern(uint8_t lanternPin)
    : pin(lanternPin) {}

void Lantern::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
}

bool Lantern::getLanternStatus() {
    return lanternStatus;
}

void Lantern::setLanternStatus(bool status) {
    lanternStatus = status;
    digitalWrite(pin, status ? LOW : HIGH);
}

void Lantern::toggle() {
    setLanternStatus(!lanternStatus);
}