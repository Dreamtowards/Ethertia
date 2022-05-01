//
// Created by Dreamtowards on 2022/5/1.
//

#ifndef ETHERTIA_GUIBUTTON_H
#define ETHERTIA_GUIBUTTON_H

#include <ethertia/client/gui/Gui.h>

#include <utility>

class GuiButton : public Gui
{
    std::string text;

public:
    GuiButton(std::string text) : text(std::move(text)), Gui(0, 0, 80, 16) {}

    void onDraw() override
    {
        Gui::drawRect(getX(), getY(), getWidth(), getHeight(), Colors::BLACK);
        Gui::drawString(getX()+getWidth()/2, getY(), text, isHover() ? Colors::RED : Colors::WHITE,
                        16, 0.5, true);

        Gui::onDraw();
    }
};

#endif //ETHERTIA_GUIBUTTON_H
