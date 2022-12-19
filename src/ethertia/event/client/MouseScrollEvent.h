//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_MOUSESCROLLEVENT_H
#define ETHERTIA_MOUSESCROLLEVENT_H

/**
 * Actually not only mouse, but also include Touchpad.
 * but the name cannot be too short. think as mouse is ok.
 */

class MouseScrollEvent
{
    float scrollDX;
    float scrollDY;

public:
    MouseScrollEvent(float _dx, float _dy) : scrollDX(_dx), scrollDY(_dy) {}

    float getScrollDX() const {
        return scrollDX;
    }
    float getScrollDY() const {
        return scrollDY;
    }
};

#endif //ETHERTIA_MOUSESCROLLEVENT_H
