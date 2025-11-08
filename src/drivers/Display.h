#pragma once
#include <U8g2lib.h>

class Display {
public:
    void begin();
    void clear();
    void display();
    void drawText(int x, int y, const char* text);
    void printCentered(const char* text, int yOffset = 0);
    void fontSet(const uint8_t *font);
    void setDisplayContrast(int contrastLevel);
    void displayOn(bool status);
    void drawBitmap(int x, int y, int w, int h, const uint8_t* bitmap);
    bool getDisplayStatus();
    void setAutoOff(unsigned long timeout);
    void handleAutoOff();
    void resetAutoOff();

private:
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2 = 
        U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE, 4, 5);
    bool statusDisplay = true;

    unsigned long autoOffTime = 0;
    unsigned long lastInteraction = 0;
};