#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"

#include "screens/animations/Animator.hpp"

enum class MenuScreenState { IDLE };

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

  int selectedIndex = 0;
  int offset = 0;
  const int numMaxVisible = 5;

  static constexpr const char *options[5] = {
      "Relogio", "Calendario", "Tela WiFi", "Tela NTP", "Lanterna"};

  static constexpr int numOptions = 5;

  MenuScreenState screenState = MenuScreenState::IDLE;
};
