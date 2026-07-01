#include "ScreenManager.hpp"
#include "core/StaticRegistry.hpp"
#include "drivers/DriverContext.hpp"
#include "services/NetworkService.hpp"

ScreenManager::ScreenManager(ScreenID id, DriverContext &ctx)
    : initialScreenID(id), driverContext(ctx) {}

void ScreenManager::begin() {
  driverContext.display->begin();
  driverContext.rtc->begin();
  driverContext.lantern->begin();

  currentScreen = createScreen(initialScreenID, driverContext);
  currentScreen->begin();
}

void ScreenManager::update() {
  NetworkService::update();
  driverContext.rtc->update();

  if (currentScreen) {
    currentScreen->update();
    ScreenID next = currentScreen->nextScreen();
    if (next != currentScreen->selfScreenID() && next != ScreenID::NONE) {
      currentScreen->end();
      currentStateScreen = 0;
      delete currentScreen;
      currentScreen = createScreen(next, driverContext);
      currentScreen->begin();
      driverContext.display->resetAutoOff();
    }

    uint8_t screenState = currentScreen->getState();

    if (screenState != currentStateScreen) {
      currentStateScreen = screenState;
      driverContext.display->resetAutoOff();
    }
  }

  driverContext.display->handleAutoOff();

  yield();
}

void ScreenManager::draw() {
  if (currentScreen)
    currentScreen->draw();
}

void ScreenManager::handleInput(ButtonEvent ev) {
  if (!driverContext.display->getDisplayStatus() && ev != ButtonEvent::None) {
    driverContext.display->displayOn(true);
    driverContext.display->resetAutoOff();
    return;
  }

  if (ev != ButtonEvent::None) {
    driverContext.display->resetAutoOff();
  }

  if (currentScreen) {
    currentScreen->handleInput(ev);
  }
}
