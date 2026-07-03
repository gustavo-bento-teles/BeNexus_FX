#include "BootScreen.hpp"
#include "screens/animations/LineGrowAnimation.hpp"

void BootScreen::begin() {
  driverContext.display->displayOn(true);
  driverContext.display->clear();

  driverContext.display->fontSet(u8g2_font_6x10_tr);
  driverContext.display->printCentered("v2.5.1-dev", 10);

  driverContext.display->fontSet(u8g2_font_9x15_mf);
  driverContext.display->printCentered("BeNexus_FX", 32);

  animator.add(new LineGrowAnimation(
      19, 36, driverContext.display->getWStr("BeNexus_FX") - 17, 5, true,
      true));

  driverContext.display->fontSet(u8g2_font_ncenB08_tr);
  driverContext.display->printCentered("by Gustavo Bento", 60);

  driverContext.display->display();
  startTime = millis();
}

void BootScreen::update() {
  if (millis() - startTime >= bootDuration) {
    nextTriggered = true;
  }

  animator.update();
}

void BootScreen::draw() {
  animator.draw(driverContext.display);

  driverContext.display->display();
}

void BootScreen::end() {
  driverContext.display->clear();
  driverContext.display->display();
  animator.clear();
}
