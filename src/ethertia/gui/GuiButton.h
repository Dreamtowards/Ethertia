//
// Created by Dreamtowards on 2022/5/1.
//

#ifndef ETHERTIA_GUIBUTTON_H
#define ETHERTIA_GUIBUTTON_H

#include <utility>

#include <ethertia/gui/Gui.h>
#include <ethertia/render/renderer/gui/FontRenderer.h>

class GuiButton : public Gui
{

    bool enableBackground = true;

public:
    bool m_Enable = true;

    std::string m_Text;

    GuiButton(std::string text, bool bg = true) : m_Text(std::move(text)), enableBackground(bg), Gui(0, 0, 100, 20) {
        glm::vec2 n = Ethertia::getRenderEngine()->fontRenderer->textBound(m_Text, 16);
        setWidth(n.x + 16);
    }

    void implDraw() override
    {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();

        if (enableBackground) {
            drawButtonBackground(this);
        }

        glm::vec4 textColor = Colors::WHITE;
        if (isHover()) textColor = Colors::YELLOW;
        if (isPressed()) textColor = Colors::GOLD;
        if (!m_Enable) textColor = Colors::GRAY;

        float textHeight = 16;
        Gui::drawString(x+w/2, y+(h-textHeight)/2, m_Text,
                        textColor,
                        textHeight, {-0.5, 0}, true);
    }

    static void drawButtonBackground(Gui* g) {
        float x = g->getX(), y = g->getY(), w = g->getWidth(), h = g->getHeight();

        if (g->isPressed()) {
            Gui::drawRect(x,y,w,h,Colors::BLACK20);
        } else if (g->isHover()) {
            Gui::drawRect(x,y,w,h,Colors::BLACK10);
        } else {
            Gui::drawRect(x,y,w,h, {
                .color = {0,0,0,0.2},
                .border = 2
            });
        }
    }
};

#endif //ETHERTIA_GUIBUTTON_H
