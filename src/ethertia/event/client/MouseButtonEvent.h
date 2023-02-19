//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_MOUSEBUTTONEVENT_H
#define ETHERTIA_MOUSEBUTTONEVENT_H

class MouseButtonEvent
{
    int button;
    int action;

public:
    MouseButtonEvent(int button, int action) : button(button), action(action) {}

    int getButton() const { return button; }

    bool isPressed() const { return action == GLFW_PRESS; }

    bool isRelease() const { return action == GLFW_RELEASE; }
};

#endif //ETHERTIA_MOUSEBUTTONEVENT_H
