#include "ScreenManager.h"
#include "services/NetworkService.h"

ScreenManager::ScreenManager(Screen* initialScreen, Display* display)
    : currentScreen(initialScreen), display(display) {}

void ScreenManager::begin() {
    if (currentScreen) currentScreen->begin();
}

void ScreenManager::update() {
    if (currentScreen) {
        currentScreen->update();
        Screen* next = currentScreen->nextScreen();
        if (next && next != currentScreen) {
            currentScreen->end();
            currentScreen = next;
            currentScreen->begin();
            display->resetAutoOff();
        }
    }
    
    NetworkService::update();

    display->handleAutoOff();
}

void ScreenManager::draw() {
    if (currentScreen) currentScreen->draw();
}

void ScreenManager::handleInput(ButtonEvent ev) {
    if (!display->getDisplayStatus() && ev != ButtonEvent::None) {
        display->displayOn(true);
        display->resetAutoOff();
        return;
    }

    if (ev != ButtonEvent::None) {
        display->resetAutoOff();
    }
    
    if (currentScreen) {
        currentScreen->handleInput(ev);
    }
}