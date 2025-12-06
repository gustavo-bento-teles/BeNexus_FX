#pragma once

class Animation {
public:
    virtual ~Animation() {}

    virtual void start() = 0;
    virtual void update() = 0;
    virtual void draw(Display* display) = 0;
    virtual bool finished() = 0;
};