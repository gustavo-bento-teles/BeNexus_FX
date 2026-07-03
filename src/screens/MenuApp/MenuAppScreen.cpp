#include "MenuAppScreen.hpp"
#include "core/StaticRegistry.hpp"
#include "screens/animations/LineGrowAnimation.hpp"

constexpr AppRegistry appsRegistry[5] = {{"Relogio", ScreenID::CLOCK},
                                         {"Calendario", ScreenID::CALENDAR},
                                         {"Tela WiFi", ScreenID::WIFI},
                                         {"Tela NTP", ScreenID::NTP},
                                         {"Lanterna", ScreenID::FLASHLIGHT}};

constexpr uint8_t numOptions = sizeof(appsRegistry) / sizeof(appsRegistry[0]);

void MenuAppScreen::begin() {
  nextTriggered = false;
  driverContext.display->fontSet(u8g2_font_6x10_tr);

  int visibleIndex = selectedIndex - offset;

  animator.add(new LineGrowAnimation(
      22, 12 + (visibleIndex * 12),
      driverContext.display->getWStr(appsRegistry[selectedIndex].optionName) -
          19,
      3, true, true));
}

void MenuAppScreen::update() { animator.update(); }

void MenuAppScreen::draw() {
  driverContext.display->clear();

  animator.draw(driverContext.display);

  driverContext.display->fontSet(u8g2_font_6x10_tr);

  int y = 10;
  for (int i = 0; i < numMaxVisible; i++) {
    int index = offset + i;
    if (index >= numOptions)
      break;

    String line = (index == selectedIndex) ? "> " : "  ";
    line += appsRegistry[index].optionName;

    driverContext.display->drawText(10, y + (i * 12), line.c_str());
  }

  driverContext.display->display();
}

void MenuAppScreen::end() {
  driverContext.display->clear();
  driverContext.display->display();
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

  animator.add(new LineGrowAnimation(
      22, 12 + (visibleIndex * 12),
      driverContext.display->getWStr(appsRegistry[selectedIndex].optionName) -
          19,
      3, true, true));
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

  animator.add(new LineGrowAnimation(
      22, 12 + (visibleIndex * 12),
      driverContext.display->getWStr(appsRegistry[selectedIndex].optionName) -
          19,
      3, true, true));
}

void MenuAppScreen::onSelectPressed() {
  nextScreenID = appsRegistry[selectedIndex].optionIdScreen;
  nextTriggered = true;
}
