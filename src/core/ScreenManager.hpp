#pragma once
#include "drivers/Display.hpp"
#include "drivers/RTC.hpp"
#include "input/ButtonEvent.hpp"
#include "screens/Screen.hpp"

class ScreenManager {
public:
  explicit ScreenManager(Screen *initialScreen, Display *display,
                         RTCManager *rtc);

  void begin();
  void update();
  void draw();
  void handleInput(ButtonEvent ev);

private:
  Screen *currentScreen;
  Display *display;
  RTCManager *rtc;

  uint8_t currentStateScreen = -1;
};
