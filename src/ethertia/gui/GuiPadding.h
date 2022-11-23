//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIPADDING_H
#define ETHERTIA_GUIPADDING_H

#include "Gui.h"
#include "GuiDelegate.h"

class GuiPadding : public GuiDelegate
{
    float left;
    float top;
    float right;
    float bottom;

    void onLayout() override
    {
        Gui* g = getContent();
        if (g) {
            g->setRelativeXY(left, top);

            setWidth(left + g->getWidth() + right);
            setHeight(top + g->getHeight() + bottom);
        }

        Gui::onLayout();
    }

public:
    glm::vec4 getPadding() {
        return glm::vec4(left, top, right, bottom);
    }

    void setPadding(glm::vec4 ltrb) {
        left = ltrb.x;
        top = ltrb.y;
        right = ltrb.z;
        bottom = ltrb.w;
    }

};

#endif //ETHERTIA_GUIPADDING_H
