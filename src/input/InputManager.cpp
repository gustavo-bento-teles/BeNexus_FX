#include "InputManager.h"

InputManager::InputManager(Buttons& b) : buttons(b) {}

void InputManager::begin() {
    buttons.begin();
}

void InputManager::update() {
    buttons.update();
}

ButtonEvent InputManager::getEvent() {
    if (buttons.upHeld())  return ButtonEvent::UpHold;
    if (buttons.downHeld())  return ButtonEvent::DownHold;
    if (buttons.selectHeld()) return ButtonEvent::SelectHold;

    if (buttons.btnUp.rose() && !buttons.getUpHoldFired())
        return ButtonEvent::Up;
    if (buttons.btnDown.rose() && !buttons.getDownHoldFired())
        return ButtonEvent::Down;
    if (buttons.btnSelect.rose() && !buttons.getSelectHoldFired())
        return ButtonEvent::Select;

    return ButtonEvent::None;
}