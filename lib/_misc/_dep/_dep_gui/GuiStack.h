//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUISTACK_H
#define ETHERTIA_GUISTACK_H

#include "GuiCollection.h"

class GuiStack : public GuiCollection
{
    int direction;
    float gap;

public:
    static constexpr int D_HORIZONTAL = 0;
    static constexpr int D_VERTICAL = 1;

    GuiStack(int dir, float gap = 0) : direction(dir), gap(gap) {}

    void onLayout() override
    {
        float dif = 0;
        for (Gui* g : children()) {
            if (direction == D_HORIZONTAL)   {
                g->setRelativeX(dif);
                dif += g->getWidth();
            } else if (direction == D_VERTICAL) {
                g->setRelativeY(dif);
                dif += g->getHeight();
            }
            dif += gap;
        }

        Gui::onLayout();
    }
};

#endif //ETHERTIA_GUISTACK_H
