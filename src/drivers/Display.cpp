#include "Display.h"

void Display::begin() {
    u8g2.begin();
    displayOn(true);
    fontSet(u8g2_font_6x10_tr);
    setDisplayContrast(0);
    setAutoOff(5000);
}

void Display::clear() {
    u8g2.clearBuffer();
}

void Display::display() {
    u8g2.sendBuffer();
}

void Display::drawText(int x, int y, const char* text) {
    u8g2.drawStr(x, y, text);
}

void Display::printCentered(const char* text, int y) {
    int width = u8g2.getStrWidth(text);
    int x = (128 - width) / 2;
    u8g2.drawStr(x, y, text);
}

void Display::fontSet(const uint8_t *font) {
    u8g2.setFont(font);
}

void Display::setDisplayContrast(int contrastLevel) {
    u8g2.setContrast(contrastLevel);
}

void Display::displayOn(bool status) {
    if (status) {
        u8g2.setPowerSave(0);
        statusDisplay = true;
    } else {
        u8g2.setPowerSave(1);
        statusDisplay = false;
    }
}

void Display::drawBitmap(int x, int y, int w, int h, const uint8_t* bitmap) {
    u8g2.drawXBMP(x, y, w, h, bitmap);
}

bool Display::getDisplayStatus() {
    return statusDisplay;
}

void Display::setAutoOff(unsigned long timeout) {
    autoOffTime = timeout;
    lastInteraction = millis();
}

void Display::handleAutoOff() {
    if (statusDisplay && autoOffTime > 0 && millis() - lastInteraction >= autoOffTime) {
        displayOn(false);
    }
}

void Display::resetAutoOff() {
    lastInteraction = millis();
    if (!statusDisplay) displayOn(true);
}