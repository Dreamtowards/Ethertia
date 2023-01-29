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

    glm::vec4 m_TextColor = Colors::WHITE;
    float m_TextHeight = 16;
    bool m_DropShadow = true;

    float m_TextX = 0;
    float m_TextY = 0;

    GuiText(std::string s, glm::vec4 col = Colors::WHITE, float hei = 16, bool shadow = true) : text(std::move(s)), m_TextColor(col), m_TextHeight(hei), m_DropShadow(shadow),
                                                                                                Gui(0, 0, 100, hei) {}

    void implDraw() override {

        Gui::drawString(getX()+m_TextX, getY()+m_TextY, text, m_TextColor, m_TextHeight, {0,0}, m_DropShadow);
    }
};

#endif //ETHERTIA_GUITEXT_H
