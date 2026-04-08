#include "ScreenManager.h"
#include "services/NetworkService.h"

ScreenManager::ScreenManager(Screen* initialScreen, Display* display, RTCManager* rtc)
    : currentScreen(initialScreen), display(display), rtc(rtc) {}

void ScreenManager::begin() {
    if (currentScreen) currentScreen->begin();
    
    rtc->begin();
}

void ScreenManager::update() {
    NetworkService::update();
    rtc->update();

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

    display->handleAutoOff();

    yield();
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