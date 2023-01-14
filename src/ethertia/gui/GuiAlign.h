//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIALIGN_H
#define ETHERTIA_GUIALIGN_H

#include "Gui.h"
#include "GuiDelegate.h"

class GuiAlign : public GuiDelegate
{
public:
    float ratioX = Inf;
    float ratioY = Inf;

    float left = Inf;  // may should NaN? for logical.
    float top  = Inf;
    float right  = Inf;
    float bottom = Inf;

    GuiAlign() {}
    GuiAlign(float ratioX, float ratioY, Gui* content,
             float _left=Inf, float _top=Inf, float _right=Inf, float _bottom=Inf) : ratioX(ratioX), ratioY(ratioY), GuiDelegate(content),
                                                                                     left(_left), top(_top), right(_right), bottom(_bottom)  {}



    void onLayout() override
    {
        if (ratioX != Inf || right != Inf) {
            setWidth(Inf);
        }
        if (ratioY != Inf || bottom != Inf) {
            setHeight(Inf);
        }

        Gui* g = getContent();
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

        GuiDelegate::onLayout();
    }

};

#endif //ETHERTIA_GUIALIGN_H
