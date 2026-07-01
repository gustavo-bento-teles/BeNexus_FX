#pragma once
#include "drivers/Display.hpp"
#include "screens/Screen.hpp"
#include <ESP8266WiFi.h>

enum class WiFiScreenState { IDLE, CONNECTED, CONNECTING, FAILED };

class WiFiScreen : public Screen {
public:
  WiFiScreen(Display *disp, Screen *next)
      : display(disp), nextScreenPtr(next), nextTriggered(false) {}

  const char *name() override { return "WiFiScreen"; }

  void begin() override;
  void update() override;
  void draw() override;
  void end() override;

  Screen *nextScreen() override { return nextTriggered ? nextScreenPtr : this; }

  uint8_t getState() const override { return static_cast<int>(screenState); }

protected:
  void onUpPressed() override;
  void onDownPressed() override;
  void onSelectPressed() override;

private:
  Display *display;
  Screen *nextScreenPtr;
  bool nextTriggered;

  WiFiScreenState screenState = WiFiScreenState::IDLE;
};
