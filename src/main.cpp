#include <Arduino.h>

#include "drivers/Display.h"
#include "drivers/RTC.h"
#include "drivers/Buttons.h"
#include "drivers/Lantern.h"

#include "screens/BootScreen.h"
#include "screens/ClockScreen.h"
#include "screens/MenuAppScreen.h"
#include "screens/WiFiScreen.h"
#include "screens/NTPScreen.h"
#include "screens/FlashlightScreen.h"

#include "core/ScreenManager.h"
#include "input/InputManager.h"
#include "output/OutputManager.h"

const uint8_t PIN_UP_BTN     = 12;
const uint8_t PIN_DOWN_BTN   = 13;
const uint8_t PIN_SELECT_BTN = 14;

const uint8_t PIN_LANTERN    = 16;

Display display;
RTCManager rtc;
Buttons buttons(PIN_UP_BTN, PIN_DOWN_BTN, PIN_SELECT_BTN);
Lantern lantern(PIN_LANTERN);

OutputManager outputManager(lantern);
InputManager inputManager(buttons);

MenuAppScreen menuAppScreen(&display);
FlashlightScreen flashlightScreen(&display, &menuAppScreen, &outputManager);
NTPScreen ntpScreen(&display, &rtc, &menuAppScreen);
WiFiScreen wifiScreen(&display, &menuAppScreen);
ClockScreen clockScreen(&display, &rtc, &menuAppScreen);
BootScreen bootScreen(&display, &menuAppScreen);

ScreenManager screenManager(&bootScreen, &display);

void setup() {
    display.begin();

    menuAppScreen.setScreens(&clockScreen, &wifiScreen, &ntpScreen, &flashlightScreen);

    screenManager.begin();
    inputManager.begin();
    outputManager.begin();
}

void loop() {
    screenManager.update();
    inputManager.update();
    outputManager.update();

    ButtonEvent ev = inputManager.getEvent();
    screenManager.handleInput(ev);

    screenManager.draw();
}