#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"
#include <ESP8266WiFi.h>

enum class WiFiScreenState { IDLE, CONNECTED, CONNECTING, FAILED };

class WiFiScreen : public Screen {
public:
  WiFiScreen(DriverContext &ctx, ScreenID next)
      : driverContext(ctx), nextScreenID(next), nextTriggered(false) {}

  const char *name() override { return "WiFiScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::WIFI; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::WIFI;
  }

  static Screen *create(DriverContext &driverContext) {
    return new WiFiScreen(driverContext, ScreenID::MENUAPP);
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

  WiFiScreenState screenState = WiFiScreenState::IDLE;
};
