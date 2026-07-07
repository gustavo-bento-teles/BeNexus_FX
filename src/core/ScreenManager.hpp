#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "input/ButtonEvent.hpp"
#include "screens/Screen.hpp"

class ScreenManager {
public:
  ScreenManager(ScreenID initialScreenID, DriverContext &driverContext);

  void begin();
  void update();
  void draw();
  void handleInput(ButtonEvent ev);

private:
  ScreenID initialScreenID;
  Screen *currentScreen;
  uint8_t currentStateScreen = 0;

  DriverContext &driverContext;
};
