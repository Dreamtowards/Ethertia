//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_MOUSEBUTTONEVENT_H
#define ETHERTIA_MOUSEBUTTONEVENT_H

class MouseButtonEvent
{
    int button;
    bool pressed;

public:
    MouseButtonEvent(int button, bool pressed) : button(button), pressed(pressed) {}

    int getButton() const { return button; }

    bool isPressed() const { return pressed; }
};

#endif //ETHERTIA_MOUSEBUTTONEVENT_H
