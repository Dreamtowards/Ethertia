//
// Created by Dreamtowards on 2022/5/1.
//

#ifndef ETHERTIA_GUIBUTTON_H
#define ETHERTIA_GUIBUTTON_H

#include <utility>

#include <ethertia/gui/Gui.h>
#include <ethertia/render/gui/FontRenderer.h>

class GuiButton : public Gui
{

    bool enableBackground = true;

public:
    bool m_Enable = true;

    std::string m_Text;

    float m_TextHeight = 16;

    GuiButton(std::string text, bool bg = true, float hei = 16, float xPadding=8) : m_Text(std::move(text)), m_TextHeight(hei), enableBackground(bg),
                                                                                    Gui(0, 0, 100, hei+4) {
        glm::vec2 n = FontRenderer::textBound(m_Text, m_TextHeight);
        setWidth(n.x + xPadding*2);
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

        Gui::drawString(x+w/2, y+(h-m_TextHeight)/2, m_Text,
                        textColor,
                        m_TextHeight, {-0.5, 0}, true);
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
