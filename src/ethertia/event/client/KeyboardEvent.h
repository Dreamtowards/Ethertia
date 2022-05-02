//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_KEYBOARDEVENT_H
#define ETHERTIA_KEYBOARDEVENT_H

class KeyboardEvent
{
    int key;
    bool pressed;

public:
    KeyboardEvent(int key, bool pressed) : key(key), pressed(pressed) {}

    int getKey() const {
        return key;
    }

    bool isPressed() const {
        return pressed;
    }
};

#endif //ETHERTIA_KEYBOARDEVENT_H
