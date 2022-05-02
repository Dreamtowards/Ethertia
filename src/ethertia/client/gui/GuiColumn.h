//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUICOLUMN_H
#define ETHERTIA_GUICOLUMN_H

#include "Gui.h"

class GuiColumn : public GuiCollection
{
public:

    void onLayout() override
    {
        float y = 0;
        for (Gui* g : children()) {
            g->setRelativeY(y);
            y += g->getHeight();
        }

        GuiCollection::onLayout();
    }

};

#endif //ETHERTIA_GUICOLUMN_H
