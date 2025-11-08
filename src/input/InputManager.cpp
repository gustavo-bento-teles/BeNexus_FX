#include "InputManager.h"

InputManager::InputManager(Buttons& b) : buttons(b) {}

void InputManager::begin() {
    buttons.begin();
}

void InputManager::update() {
    buttons.update();
}

ButtonEvent InputManager::getEvent() {
    if (buttons.upPressed()) return ButtonEvent::Up;
    if (buttons.downPressed()) return ButtonEvent::Down;
    if (buttons.selectPressed()) return ButtonEvent::Select;

    return ButtonEvent::None;
}