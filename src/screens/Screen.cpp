#include "Screen.h"

void Screen::handleInput(ButtonEvent ev) {
    switch (ev) {
        case ButtonEvent::Up: onUpPressed(); break;
        case ButtonEvent::Down: onDownPressed(); break;
        case ButtonEvent::Select: onSelectPressed(); break;

        case ButtonEvent::UpHold: onUpHeld(); break;
        case ButtonEvent::DownHold: onDownHeld(); break;
        case ButtonEvent::SelectHold: onSelectHeld(); break;
        default: break;
    }
}