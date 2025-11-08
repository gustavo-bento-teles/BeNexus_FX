#include "output/OutputManager.h"

OutputManager::OutputManager(Lantern& l)
    : lantern(l) {}

void OutputManager::begin() {
    lantern.begin();
}

void OutputManager::update() {}

void OutputManager::setLantern(bool on) {
    lantern.setLanternStatus(on);
}

void OutputManager::toggleLantern() {
    lantern.toggle();
}

bool OutputManager::isLanternOn() const {
    return lantern.getLanternStatus();
}