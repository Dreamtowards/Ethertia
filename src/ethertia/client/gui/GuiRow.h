//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIROW_H
#define ETHERTIA_GUIROW_H

#include "Gui.h"

class GuiRow : public GuiCollection
{
public:

    void onLayout() override
    {
        float x = 0;
        for (Gui* g : children()) {
            g->setRelativeX(x);
            x += g->getWidth();
        }

        Gui::onLayout();
    }
};

#endif //ETHERTIA_GUIROW_H
