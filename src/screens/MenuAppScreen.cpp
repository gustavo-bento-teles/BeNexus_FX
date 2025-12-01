#include "MenuAppScreen.h"

void MenuAppScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
    display->display();

    animator.add(new LineGrowAnimation(22, 12 + (selectedIndex * 12), display->getWStr(options[selectedIndex]) - 19, 3, true, true));
}

void MenuAppScreen::update() {
    animator.update();
}

void MenuAppScreen::draw() {
    display->clear();

    animator.draw(display);

    display->fontSet(u8g2_font_6x10_tr);

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
    animator.clear();
}

void MenuAppScreen::onUpPressed() {
    if (selectedIndex > 0) {
        selectedIndex--;
    } else {
        selectedIndex = numOptions - 1;
    }

    animator.add(new LineGrowAnimation(22, 12 + (selectedIndex * 12), display->getWStr(options[selectedIndex]) - 19, 3, true, true));
}

void MenuAppScreen::onDownPressed() {
    if (selectedIndex < numOptions - 1) {
        selectedIndex++;
    } else {
        selectedIndex = 0;
    }

    animator.add(new LineGrowAnimation(22, 12 + (selectedIndex * 12), display->getWStr(options[selectedIndex]) - 19, 3, true, true));
}

void MenuAppScreen::onSelectPressed() {
    switch (selectedIndex) {
        case 0:
            nextScreenPtr = clockScreen;
            nextTriggered = true;
            break;
        case 1:
            nextScreenPtr = calendarScreen;
            nextTriggered = true;
            break;
        case 2:
            nextScreenPtr = wifiScreen;
            nextTriggered = true;
            break;
        case 3:
            nextScreenPtr = ntpScreen;
            nextTriggered = true;
            break;
        case 4:
            nextScreenPtr = flashlightScreen;
            nextTriggered = true;
            break;
    }
}