#pragma once
#include "ButtonEvent.hpp"
#include "drivers/DriverContext.hpp"

class InputManager {
public:
  InputManager(DriverContext &driverContext);

  void begin();
  void update();
  ButtonEvent getEvent();

private:
  DriverContext &driverContext;
};
