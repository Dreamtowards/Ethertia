//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_WINDOWRESIZEDEVENT_H
#define ETHERTIA_WINDOWRESIZEDEVENT_H

class WindowResizedEvent
{
    float width;
    float height;

public:
    WindowResizedEvent(float width, float height) : width(width), height(height) {}

    float getWidth() const {
        return width;
    }

    float getHeight() const {
        return height;
    }
};

#endif //ETHERTIA_WINDOWRESIZEDEVENT_H
