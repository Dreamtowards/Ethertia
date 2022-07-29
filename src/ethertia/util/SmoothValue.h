//
// Created by Dreamtowards on 2022/7/29.
//

#ifndef ETHERTIA_SMOOTHVALUE_H
#define ETHERTIA_SMOOTHVALUE_H

#include <ethertia/util/Mth.h>

class SmoothValue
{
public:
    float target = 0;

    float current = 0;

    void update(float dt) {

        current = Mth::lerp(dt, current, target);
    }


};

#endif //ETHERTIA_SMOOTHVALUE_H
