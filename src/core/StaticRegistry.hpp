#pragma once
#include "drivers/DriverContext.hpp"

class Screen;

enum class ScreenID {
  NONE,
  BOOT,
  CALENDAR,
  CLOCK,
  FLASHLIGHT,
  MENUAPP,
  NTP,
  WIFI
};

struct ScreenRegistry {
  ScreenID id;

  Screen *(*creator)(DriverContext &);
};

Screen *createScreen(ScreenID id, DriverContext &ctx);
