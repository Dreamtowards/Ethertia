//
// Created by Dreamtowards on 2023/1/29.
//

#ifndef ETHERTIA_GUISEPARATOR_H
#define ETHERTIA_GUISEPARATOR_H

#include <ethertia/gui/Gui.h>

class GuiSeparator : public Gui
{
public:
    float m_Thickness = 1;

    GuiSeparator() : Gui(0, 0, Inf, 6) {

    }

    void implDraw() override
    {
        float x=getX(), y=getY(), w=getWidth(), h=getHeight();

        drawRect(x, y + (h-m_Thickness)/2, w, m_Thickness, Colors::BLACK20);
    }
};

#endif //ETHERTIA_GUISEPARATOR_H
