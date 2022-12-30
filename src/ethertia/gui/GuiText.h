//
// Created by Dreamtowards on 2022/8/12.
//

#ifndef ETHERTIA_GUITEXT_H
#define ETHERTIA_GUITEXT_H

#include "Gui.h"

class GuiText : public Gui
{
public:

    std::string text;

    GuiText(std::string s) : text(std::move(s)), Gui(0, 0, 100, 16) {}

    void implDraw() override {

        Gui::drawString(getX(), getY(), text);
    }
};

#endif //ETHERTIA_GUITEXT_H
