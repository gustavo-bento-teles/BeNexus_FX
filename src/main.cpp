#include <Arduino.h>

#include "core/ScreenManager.hpp"
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "input/InputManager.hpp"

DriverContext driverContext = initDriverContext();

InputManager inputManager(driverContext);

ScreenManager screenManager(ScreenID::BOOT, driverContext);

void setup() {
  screenManager.begin();
  inputManager.begin();
}

void loop() {
  screenManager.update();
  inputManager.update();

  ButtonEvent ev = inputManager.getEvent();
  screenManager.handleInput(ev);

  screenManager.draw();
}
