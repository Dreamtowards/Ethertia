//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIALIGN_H
#define ETHERTIA_GUIALIGN_H

#include "Gui.h"

class GuiAlign : public Gui, public Gui::Contentable
{
public:
    float ratioX = Inf;
    float ratioY = Inf;

    float left = Inf;  // may should NaN? for logical.
    float top  = Inf;
    float right  = Inf;
    float bottom = Inf;

    GuiAlign() : Gui(0, 0, Inf, Inf) { }

    GuiAlign(float ratioX, float ratioY) : ratioX(ratioX), ratioY(ratioY), Gui(0, 0, Inf, Inf) { }

    void onLayout() override
    {
        Gui* g = getGui(0);
        if (g) {
            if (ratioX != Inf) g->setRelativeX((getWidth() - g->getWidth()) * ratioX);
            if (ratioY != Inf) g->setRelativeY((getHeight() - g->getHeight()) * ratioY);

            if (left != Inf) g->setRelativeX(left);
            if (top != Inf) g->setRelativeY(top);

            if (right != Inf) {
                if (left != Inf || ratioX != Inf) g->setWidth(getWidth() - g->getRelativeX() - right);
                else g->setRelativeX(getWidth() - g->getWidth() - right);
            }
            if (bottom != Inf) {
                if (top != Inf || ratioY != Inf) g->setHeight(getHeight() - g->getRelativeY() - bottom);
                else g->setRelativeY(getHeight() - g->getHeight() - bottom);
            }
        }

        Gui::onLayout();
    }


    // Duplicated Code.
    Gui* getContent() override {
        return count() ? getGui(0) : nullptr;
    }

    Gui* setContent(Gui* content) override {
        removeAllGuis();
        addGui(content);
        return this;
    }
};

#endif //ETHERTIA_GUIALIGN_H
