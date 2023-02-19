//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CONTROLS_H
#define ETHERTIA_CONTROLS_H


class Controls
{
public:

    // MBR held time of Current Item.
    inline static float g_CurrentItemUsedTime = 0;

    static void initControls();

    static void handleContinuousInput();

    static void saveScreenshot();

};

#endif //ETHERTIA_CONTROLS_H
