//
// Created by Dreamtowards on 2022/5/1.
//

#ifndef ETHERTIA_GUIBUTTON_H
#define ETHERTIA_GUIBUTTON_H

#include <utility>

#include <ethertia/client/gui/Gui.h>
#include <ethertia/client/render/renderer/gui/FontRenderer.h>

class GuiButton : public Gui
{
    std::string text;

public:
    GuiButton(std::string text) : text(std::move(text)), Gui(0, 0, 100, 20) {
        glm::vec2 n = Ethertia::getRenderEngine()->fontRenderer->textBound(this->text, 16);
        setWidth(n.x + 16);
    }

    void onDraw() override
    {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();

        drawButtonBackground(this);

        Gui::drawString(x+w/2, y+(h-16)/2, text,
                        isPressed() ? Colors::GOLD : isHover() ? Colors::YELLOW : Colors::WHITE,
                        16, 0.5, true);

        Gui::onDraw();
    }

    static void drawButtonBackground(Gui* g) {
        float x = g->getX(), y = g->getY(), w = g->getWidth(), h = g->getHeight();

        if (g->isPressed()) {
            Gui::drawRect(x,y,w,h,Colors::BLACK20);
        } else if (g->isHover()) {
            Gui::drawRect(x,y,w,h,Colors::BLACK10);
        }
    }
};

#endif //ETHERTIA_GUIBUTTON_H
