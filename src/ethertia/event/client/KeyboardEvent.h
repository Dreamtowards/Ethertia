//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_KEYBOARDEVENT_H
#define ETHERTIA_KEYBOARDEVENT_H

class KeyboardEvent
{
    int key;
    int action;

public:
    KeyboardEvent(int key, bool action) : key(key), action(action) {}

    int getKey() const {
        return key;
    }

    bool isPressed() const {
        return action == GLFW_PRESS;
    }
    bool isReleased() const {
        return action == GLFW_RELEASE;
    }
};

#endif //ETHERTIA_KEYBOARDEVENT_H
