#include "DriverContext.hpp"

const uint8_t PIN_UP_BTN = 12;
const uint8_t PIN_DOWN_BTN = 13;
const uint8_t PIN_SELECT_BTN = 14;

const uint8_t PIN_LANTERN = 16;

Display display;
RTCManager rtc;
Buttons buttons(PIN_UP_BTN, PIN_DOWN_BTN, PIN_SELECT_BTN);
Lantern lantern(PIN_LANTERN);

DriverContext initDriverContext() {
  return {&display, &rtc, &buttons, &lantern};
}
