//
// Created by Dreamtowards on 2023/1/14.
//

#ifndef ETHERTIA_GUIF4LOCK_H
#define ETHERTIA_GUIF4LOCK_H

#include <ethertia/gui/GuiCommon.h>

class GuiF4Lock : public Gui
{
public:
    DECL_Inst_(GuiF4Lock);

    void implDraw() override
    {
        float h = 16;
        float y = Gui::maxHeight()-h;
        Gui::drawRect(0, y, Gui::maxWidth(), h, Colors::R);

        Gui::drawString(0, y, "FORCE LOCKDOWN");
    }

};

#endif //ETHERTIA_GUIF4LOCK_H
