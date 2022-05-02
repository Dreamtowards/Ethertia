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
    GuiButton(std::string text) : text(std::move(text)), Gui(0, 0, 100, 20) {}

    void onDraw() override
    {
        Gui::drawRect(getX(), getY(), getWidth(), getHeight(),
                      isPressed() ? Colors::BLACK80 : Colors::BLACK, nullptr, 3.0);
        Gui::drawRect(getX(), getY(), getWidth(), getHeight(),
                      isPressed() ? Colors::RED : isHover() ? Colors::GRAY : Colors::WHITE30, nullptr, 3, 2);

        Gui::drawString(getX()+getWidth()/2, getY()+(getHeight()-16)/2, text,
                        isPressed() ? Colors::RED : isHover() ? Colors::YELLOW : Colors::WHITE,
                        16, 0.5, true);

        Gui::onDraw();
    }
};

#endif //ETHERTIA_GUIBUTTON_H
