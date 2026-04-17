#include "MenuAppScreen.h"

void MenuAppScreen::begin() {
    nextTriggered = false;
    display->fontSet(u8g2_font_6x10_tr);

    int visibleIndex = selectedIndex - offset;

    animator.add(
        new LineGrowAnimation(
            22,
            12 + (visibleIndex * 12),
            display->getWStr(options[selectedIndex]) - 19,
            3,
            true,
            true
        )
    );
}

void MenuAppScreen::update() {
    animator.update();
}

void MenuAppScreen::draw() {
    display->clear();

    animator.draw(display);

    display->fontSet(u8g2_font_6x10_tr);

    int y = 10;
    for (int i = 0; i < numMaxVisible; i++) {
        int index = offset + i;
        if (index >= numOptions) break;

        String line = (index == selectedIndex) ? "> " : "  ";
        line += options[index];

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
        if (selectedIndex < offset) {
            offset--;
        }
    } else {
        selectedIndex = numOptions - 1;
        offset = max(0, numOptions - numMaxVisible);
    }

    int visibleIndex = selectedIndex - offset;

    animator.add(
        new LineGrowAnimation(
            22,
            12 + (visibleIndex * 12),
            display->getWStr(options[selectedIndex]) - 19,
            3,
            true,
            true
        )
    );
}

void MenuAppScreen::onDownPressed() {
    if (selectedIndex < numOptions - 1) {
        selectedIndex++;
        if (selectedIndex > (numMaxVisible - 1 + offset)) {
            offset++;
        }
    } else {
        selectedIndex = 0;
        offset = 0;
    }

    int visibleIndex = selectedIndex - offset;

    animator.add(
        new LineGrowAnimation(
            22,
            12 + (visibleIndex * 12),
            display->getWStr(options[selectedIndex]) - 19,
            3,
            true,
            true
        )
    );
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