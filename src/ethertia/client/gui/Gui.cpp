//
// Created by Dreamtowards on 2022/4/29.
//

#include "Gui.h"
#include <ethertia/client/Ethertia.h>

float Gui::maxWidth() {
    return Ethertia::getWindow()->getWidth();
}

float Gui::maxHeight() {
    return Ethertia::getWindow()->getHeight();
}

bool Gui::isMouseOver() {
    auto* w = Ethertia::getWindow();
    return isPointOver(w->getMouseX(), w->getMouseY());
}


void Gui::drawRect(float x, float y, float w, float h, glm::vec4 color, Texture* tex, float round) {
    auto& gr = Ethertia::getRenderEngine()->guiRenderer;
    gr.render(x, y, w, h, color, tex);
}

void Gui::drawString(float x, float y, const std::string &str, glm::vec4 color, float textHeight, float alignX, bool dropShadow) {
    auto& fr = Ethertia::getRenderEngine()->fontRenderer;
    if (dropShadow) {
        float off = textHeight / 10;
        auto scolor = color * 0.2f; scolor.a = color.a;
        fr.renderString(x+off, y+off, str, scolor, textHeight, alignX);
    }
    fr.renderString(x, y, str, color, textHeight, alignX);
}