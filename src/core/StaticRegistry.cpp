#include "core/StaticRegistry.hpp"

#include "screens/Boot/BootScreen.hpp"
#include "screens/Calendar/CalendarScreen.hpp"
#include "screens/Clock/ClockScreen.hpp"
#include "screens/Flashlight/FlashlightScreen.hpp"
#include "screens/MenuApp/MenuAppScreen.hpp"
#include "screens/NTP/NTPScreen.hpp"
#include "screens/WiFi/WiFiScreen.hpp"

template <typename ScreenName>
constexpr ScreenRegistry makeScreenRegistry(ScreenID id) {
  return {id, &ScreenName::create};
}

static const ScreenRegistry registry[] = {
    makeScreenRegistry<BootScreen>(ScreenID::BOOT),
    makeScreenRegistry<CalendarScreen>(ScreenID::CALENDAR),
    makeScreenRegistry<ClockScreen>(ScreenID::CLOCK),
    makeScreenRegistry<FlashlightScreen>(ScreenID::FLASHLIGHT),
    makeScreenRegistry<MenuAppScreen>(ScreenID::MENUAPP),
    makeScreenRegistry<NTPScreen>(ScreenID::NTP),
    makeScreenRegistry<WiFiScreen>(ScreenID::WIFI),
};

Screen *createScreen(ScreenID id, DriverContext &ctx) {
  for (const auto &entry : registry) {
    if (entry.id == id) {
      return entry.creator(ctx);
    }
  }
  return nullptr;
}
