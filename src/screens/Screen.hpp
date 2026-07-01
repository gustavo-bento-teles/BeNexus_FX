#pragma once
#include "core/StaticRegistry.hpp"
#include "input/ButtonEvent.hpp"

class Screen {
public:
  virtual ~Screen() {}

  virtual const char *name() { return "Unnamed"; }

  virtual void begin() {};
  virtual void end() {}
  virtual void update() {}
  virtual void draw() = 0;
  virtual void handleInput(ButtonEvent ev);

  virtual ScreenID selfScreenID() const { return ScreenID::NONE; }
  virtual ScreenID nextScreen() const { return ScreenID::NONE; }

  virtual uint8_t getState() const = 0;

protected:
  virtual void onUpPressed() {}
  virtual void onDownPressed() {}
  virtual void onSelectPressed() {}

  virtual void onUpHeld() {}
  virtual void onDownHeld() {}
  virtual void onSelectHeld() {}
};
