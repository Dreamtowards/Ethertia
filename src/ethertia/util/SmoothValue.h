//
// Created by Dreamtowards on 2022/7/29.
//

#pragma once

#include <cmath>

class SmoothValue
{
public:

    float target = 0;

    float current = 0;

    float update(float dt) 
    {
        float old = current;
        current = std::lerp(current, target, dt);  //  a + t * (b - a);
        return current - old;
    }

};

