#include "InputManager.hpp"

InputManager::InputManager(DriverContext &ctx) : driverContext(ctx) {}

void InputManager::begin() { driverContext.buttons->begin(); }

void InputManager::update() { driverContext.buttons->update(); }

ButtonEvent InputManager::getEvent() {
  if (driverContext.buttons->upHeld())
    return ButtonEvent::UpHold;
  if (driverContext.buttons->downHeld())
    return ButtonEvent::DownHold;
  if (driverContext.buttons->selectHeld())
    return ButtonEvent::SelectHold;

  if (driverContext.buttons->upPressed())
    return ButtonEvent::Up;
  if (driverContext.buttons->downPressed())
    return ButtonEvent::Down;
  if (driverContext.buttons->selectPressed())
    return ButtonEvent::Select;

  return ButtonEvent::None;
}
