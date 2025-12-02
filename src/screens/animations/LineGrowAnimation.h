#pragma once
#include "Animation.h"

class LineGrowAnimation : public Animation {
private:
    int x, y;
    int currentWidth = 0;
    int maxWidth;
    int speed;
    bool active = false;
    bool stopAtEnd;
    bool removeOnFinish;

public:
    LineGrowAnimation(int x, int y, int maxWidth, int speed, bool stopAtEnd, bool removeOnFinish)
        : x(x), y(y), maxWidth(maxWidth), speed(speed),
          stopAtEnd(stopAtEnd), removeOnFinish(removeOnFinish) {}

    void start() override {
        currentWidth = 0;
        active = true;
    }

    void update() override {
        if (!active) return;

        currentWidth += speed;

        if (speed > 0 && currentWidth >= maxWidth) {
            currentWidth = maxWidth;
            if (stopAtEnd) active = false;
        }
        else if (speed < 0 && currentWidth <= -maxWidth) {
            currentWidth = -maxWidth;
            if (stopAtEnd) active = false;
        }
    }

    void draw(Display* display) override {
        int x1 = x;
        int x2 = x + currentWidth;

        if (x2 < x1)
            std::swap(x1, x2);

        display->drawHorizontalLine(x1, y, x2);
    }

    bool finished() override {
        if (removeOnFinish)
            return !active;
        return false;
    }
};