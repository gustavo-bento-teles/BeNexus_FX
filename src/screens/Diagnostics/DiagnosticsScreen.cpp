#include "screens/Diagnostics/DiagnosticsScreen.h"

void DiagnosticsScreen::begin() {
    nextTriggered = false;
    display->clear();
    display->fontSet(u8g2_font_6x10_tr);
    display->display();

    int visibleIndex = selectedIndex - offset;
}

void DiagnosticsScreen::update() {}

void DiagnosticsScreen::draw() {
    display->clear();

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

void DiagnosticsScreen::end() {
    display->clear();
    display->display();
}

void DiagnosticsScreen::onUpPressed() {
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
}

void DiagnosticsScreen::onDownPressed() {
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
}

void DiagnosticsScreen::onSelectPressed() {
    nextScreenPtr = rtcDiagnostics;
    nextTriggered = true;
}