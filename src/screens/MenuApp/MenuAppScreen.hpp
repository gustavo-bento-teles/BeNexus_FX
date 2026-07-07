#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"

#include "screens/animations/Animator.hpp"

enum class MenuScreenState { IDLE };

struct AppRegistry {
  const char *optionName;
  ScreenID optionIdScreen;
};

class MenuAppScreen : public Screen {
public:
  MenuAppScreen(DriverContext &ctx)
      : driverContext(ctx), nextScreenID(ScreenID::MENUAPP),
        nextTriggered(false) {}

  const char *name() override { return "MenuAppScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::MENUAPP; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::MENUAPP;
  }

  static Screen *create(DriverContext &driverContext) {
    return new MenuAppScreen(driverContext);
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

  Animator animator;

  static int selectedIndex;
  static int offset;

  const int numMaxVisible = 5;

  MenuScreenState screenState = MenuScreenState::IDLE;
};
