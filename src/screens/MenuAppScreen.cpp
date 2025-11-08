#include "MenuAppScreen.h"

void MenuAppScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
    display->display();
}

void MenuAppScreen::update() {}

void MenuAppScreen::draw() {
    display->clear();

    display->fontSet(u8g2_font_6x10_tr);

    // posição vertical inicial
    int y = 10;
    for (int i = 0; i < numOptions; i++) {
        String line = (i == selectedIndex) ? "> " : "  ";
        line += options[i];
        display->drawText(10, y + (i * 12), line.c_str());
    }

    display->display();
}

void MenuAppScreen::end() {
    display->clear();
    display->display();
}

void MenuAppScreen::onUpPressed() {
    if (selectedIndex > 0) {
        selectedIndex--;
    } else {
        selectedIndex = numOptions - 1;
    }
}

void MenuAppScreen::onDownPressed() {
    if (selectedIndex < numOptions - 1) {
        selectedIndex++;
    } else {
        selectedIndex = 0;
    }
}

void MenuAppScreen::onSelectPressed() {
    switch (selectedIndex) {
        case 0:
            nextScreenPtr = clockScreen;
            nextTriggered = true;
            break;
        case 1:
            nextScreenPtr = wifiScreen;
            nextTriggered = true;
            break;
        case 2:
            nextScreenPtr = ntpScreen;
            nextTriggered = true;
            break;
        case 3:
            nextScreenPtr = flashlightScreen;
            nextTriggered = true;
            break;
        case 4:
            display->displayOn(false);
            selectedIndex = 0;
            break;
    }
}