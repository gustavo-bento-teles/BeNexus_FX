#include <Arduino.h>

#include "drivers/Buttons.hpp"
#include "drivers/Display.hpp"
#include "drivers/Lantern.hpp"
#include "drivers/RTC.hpp"

#include "screens/Boot/BootScreen.hpp"
#include "screens/Calendar/CalendarScreen.hpp"
#include "screens/Clock/ClockScreen.hpp"
#include "screens/Flashlight/FlashlightScreen.hpp"
#include "screens/MenuApp/MenuAppScreen.hpp"
#include "screens/NTP/NTPScreen.hpp"
#include "screens/WiFi/WiFiScreen.hpp"

#include "core/ScreenManager.hpp"
#include "input/InputManager.hpp"
#include "output/OutputManager.hpp"

const uint8_t PIN_UP_BTN = 12;
const uint8_t PIN_DOWN_BTN = 13;
const uint8_t PIN_SELECT_BTN = 14;

const uint8_t PIN_LANTERN = 16;

Display display;
RTCManager rtc;
Buttons buttons(PIN_UP_BTN, PIN_DOWN_BTN, PIN_SELECT_BTN);
Lantern lantern(PIN_LANTERN);

OutputManager outputManager(lantern);
InputManager inputManager(buttons);

MenuAppScreen menuAppScreen(&display);
FlashlightScreen flashlightScreen(&display, &menuAppScreen, &outputManager);
WiFiScreen wifiScreen(&display, &menuAppScreen);
NTPScreen ntpScreen(&display, &rtc, &menuAppScreen, &wifiScreen);
CalendarScreen calendarScreen(&display, &rtc, &menuAppScreen);
ClockScreen clockScreen(&display, &rtc, &menuAppScreen);
BootScreen bootScreen(&display, &menuAppScreen);

ScreenManager screenManager(&bootScreen, &display, &rtc);

void setup() {
  display.begin();

  menuAppScreen.setScreens(&clockScreen, &calendarScreen, &wifiScreen,
                           &ntpScreen, &flashlightScreen);

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
