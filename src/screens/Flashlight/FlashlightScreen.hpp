#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"

enum class FlashScreenState { IDLE };

class FlashlightScreen : public Screen {
public:
  FlashlightScreen(DriverContext &ctx, ScreenID next)
      : driverContext(ctx), nextScreenID(next), nextTriggered(false) {}

  const char *name() override { return "FlashlightScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::FLASHLIGHT; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::FLASHLIGHT;
  }

  static Screen *create(DriverContext &driverContext) {
    return new FlashlightScreen(driverContext, ScreenID::MENUAPP);
  }

  uint8_t getState() const override { return static_cast<int>(screenState); }

protected:
  void onUpPressed() override;
  void onDownPressed() override;
  void onSelectPressed() override;

private:
  DriverContext &driverContext;
  ScreenID nextScreenID;
  bool nextTriggered;

  FlashScreenState screenState = FlashScreenState::IDLE;
};
