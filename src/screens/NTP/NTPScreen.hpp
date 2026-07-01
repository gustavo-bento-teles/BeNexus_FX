#pragma once
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "screens/Screen.hpp"
#include <NTPClient.h>
#include <WiFiUdp.h>

enum class NTPScreenState { CONNECTED, DISCONNECTED, UPDATING };

class NTPScreen : public Screen {
public:
  NTPScreen(DriverContext &ctx, ScreenID next)
      : driverContext(ctx), nextScreenID(next), nextTriggered(false) {}

  const char *name() override { return "NTPScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  ScreenID selfScreenID() const override { return ScreenID::NTP; }

  ScreenID nextScreen() const override {
    return nextTriggered ? nextScreenID : ScreenID::NTP;
  }

  static Screen *create(DriverContext &driverContext) {
    return new NTPScreen(driverContext, ScreenID::MENUAPP);
  }

  uint8_t getState() const override { return static_cast<int>(screenState); }

protected:
  void onUpPressed() override;
  void onDownPressed() override;
  void onSelectPressed() override;

private:
  void atualizarNTP();

  DriverContext &driverContext;
  ScreenID nextScreenID;

  bool nextTriggered;
  bool firstClient = true;

  WiFiUDP ntpUDP;
  NTPClient timeClient = NTPClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

  NTPScreenState screenState = NTPScreenState::DISCONNECTED;
};
