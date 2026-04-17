#include "BootScreen.h"

void BootScreen::begin() {
    display->displayOn(true);
    display->clear();

    display->fontSet(u8g2_font_6x10_tr);
    display->printCentered("v2.4.0-stable", 10);
    
    display->fontSet(u8g2_font_9x15_mf);
    display->printCentered("BeNexus_FX", 32);

    animator.add(new LineGrowAnimation(19, 36, display->getWStr("BeNexus_FX") - 17, 5, true, true));

    display->fontSet(u8g2_font_ncenB08_tr);
    display->printCentered("by Gustavo Bento", 60);
    
    display->display();
    startTime = millis();
}

void BootScreen::update() {
    if (millis() - startTime >= bootDuration) {
        nextTriggered = true;
    }

    animator.update();
}

void BootScreen::draw() {
    animator.draw(display);

    display->display();
}

void BootScreen::end() {
    display->clear();
    display->display();
    animator.clear();
}