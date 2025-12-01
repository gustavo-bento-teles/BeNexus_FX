#pragma once
#include <vector>
#include "Animation.h"

class Animator {
private:
    std::vector<Animation*> animations;

public:
    void add(Animation* anim) {
        anim->start();
        animations.push_back(anim);
    }

    void update() {
        for (int i = animations.size() - 1; i >= 0; i--) {
            animations[i]->update();
            if (animations[i]->finished()) {
                delete animations[i];
                animations.erase(animations.begin() + i);
            }
        }
    }

    void draw(Display* display) {
        for (auto* anim : animations) {
            anim->draw(display);
        }
    }

    void clear() {
        for (auto* anim : animations) delete anim;
        animations.clear();
    }

    ~Animator() {
        clear();
    }
};