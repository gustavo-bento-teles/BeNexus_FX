#pragma once
#include "ButtonEvent.hpp"
#include "drivers/Buttons.hpp"

class InputManager {
public:
  explicit InputManager(Buttons &btns);

  void begin();
  void update();
  ButtonEvent getEvent();

private:
  Buttons &buttons;
};
