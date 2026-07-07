#pragma once

#include "drivers/Buttons.hpp"
#include "drivers/Display.hpp"
#include "drivers/Lantern.hpp"
#include "drivers/RTC.hpp"

struct DriverContext {
  Display *display;
  RTC *rtc;
  Buttons *buttons;
  Lantern *lantern;
};

extern Display display;
extern RTC rtc;
extern Buttons buttons;
extern Lantern lantern;

DriverContext initDriverContext();
