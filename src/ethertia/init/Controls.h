//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CONTROLS_H
#define ETHERTIA_CONTROLS_H


class Controls
{
public:

    inline static float gDigSpeedMultiplier = 8.0f;

    static void initConsoleThread();

    static void handleInput();

    static void saveScreenshot();

};

#endif //ETHERTIA_CONTROLS_H
