#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"

enum class ClockScreenState { IDLE };

class ClockScreen : public Screen {
public:
  ClockScreen(DriverContext &ctx, ScreenID next)
      : driverContext(ctx), nextScreenID(next), nextTriggered(false) {}

  const char *name() override { return "ClcokScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::CLOCK; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::CLOCK;
  }

  static Screen *create(DriverContext &driverContext) {
    return new ClockScreen(driverContext, ScreenID::MENUAPP);
  }

  uint8_t getState() const override { return static_cast<int>(screenState); }

protected:
  void onSelectPressed() override;
  void onUpPressed() override;
  void onDownPressed() override;

private:
  DriverContext &driverContext;
  ScreenID nextScreenID;
  bool nextTriggered;

  ClockScreenState screenState = ClockScreenState::IDLE;
};
