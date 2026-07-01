#include "core/StaticRegistry.hpp"

#include "screens/Boot/BootScreen.hpp"
#include "screens/Calendar/CalendarScreen.hpp"
#include "screens/Clock/ClockScreen.hpp"
#include "screens/Flashlight/FlashlightScreen.hpp"
#include "screens/MenuApp/MenuAppScreen.hpp"
#include "screens/NTP/NTPScreen.hpp"
#include "screens/WiFi/WiFiScreen.hpp"

static const ScreenRegistry registry[] = {
    {ScreenID::BOOT, BootScreen::create},
    {ScreenID::CALENDAR, CalendarScreen::create},
    {ScreenID::CLOCK, ClockScreen::create},
    {ScreenID::FLASHLIGHT, FlashlightScreen::create},
    {ScreenID::MENUAPP, MenuAppScreen::create},
    {ScreenID::NTP, NTPScreen::create},
    {ScreenID::WIFI, WiFiScreen::create},
};

Screen *createScreen(ScreenID id, DriverContext &ctx) {
  for (const auto &entry : registry) {
    if (entry.id == id) {
      return entry.creator(ctx);
    }
  }
  return nullptr;
}
