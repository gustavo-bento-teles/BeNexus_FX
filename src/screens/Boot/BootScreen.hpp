#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"

#include "screens/animations/Animator.hpp"

enum class BootScreenState { IDLE };

class BootScreen : public Screen {
public:
  BootScreen(DriverContext &ctx, ScreenID next)
      : driverContext(ctx), nextScreenID(next) {}

  const char *name() override { return "BootScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::BOOT; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::BOOT;
  }

  static Screen *create(DriverContext &driverContext) {
    return new BootScreen(driverContext, ScreenID::MENUAPP);
  }

  uint8_t getState() const override { return static_cast<int>(screenState); }

private:
  Animator animator;

  DriverContext &driverContext;

  ScreenID nextScreenID;
  bool nextTriggered = false;
  unsigned long startTime = 0;
  const unsigned long bootDuration = 500;

  BootScreenState screenState = BootScreenState::IDLE;
};
