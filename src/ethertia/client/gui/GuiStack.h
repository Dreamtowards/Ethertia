//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUISTACK_H
#define ETHERTIA_GUISTACK_H

#include "GuiCollection.h"

class GuiStack : public GuiCollection
{
    glm::vec2 direction;
    glm::vec2 gap;

public:
    GuiStack(float dirX, float dirY) : direction(glm::vec2(dirX, dirY)) {}
    GuiStack(float dirX, float dirY, float gapX, float gapY) : direction(glm::vec2(dirX, dirY)), gap(glm::vec2(gapX, gapY)) {}

    void onLayout() override
    {
        float x = 0;
        float y = 0;
        for (Gui* g : children()) {
            g->setRelativeX(x);
            g->setRelativeY(y);

            x += g->getWidth() * direction.x + gap.x;
            y += g->getHeight() * direction.y + gap.y;
        }

        Gui::onLayout();
    }
};

#endif //ETHERTIA_GUISTACK_H
