#pragma once
#include "drivers/Display.hpp"
#include "screens/Screen.hpp"

#include "screens/animations/Animator.hpp"
#include "screens/animations/LineGrowAnimation.hpp"

enum class BootScreenState { IDLE };

class BootScreen : public Screen {
public:
  BootScreen(Display *disp, Screen *next)
      : display(disp), nextScreenPtr(next) {}

  const char *name() override { return "BootScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  Screen *nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

  uint8_t getState() const override { return static_cast<int>(screenState); }

private:
  Animator animator;

  Display *display;
  Screen *nextScreenPtr;
  bool nextTriggered = false;
  unsigned long startTime = 0;
  const unsigned long bootDuration = 500;

  BootScreenState screenState = BootScreenState::IDLE;
};
