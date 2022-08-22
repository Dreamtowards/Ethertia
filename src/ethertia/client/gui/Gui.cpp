//
// Created by Dreamtowards on 2022/8/22.
//

#include "Gui.h"
#include <ethertia/client/render/renderer/gui/FontRenderer.h>


void Gui::drawRect(float x, float y, float w, float h, glm::vec4 color, Texture* tex, float round, float border) {
    GuiRenderer* gr = Ethertia::getRenderEngine()->guiRenderer;
    gr->render(x, y, w, h, color, tex, round, border);
}


void Gui::drawString(float x, float y, const std::string &str, glm::vec4 color, float textHeight, float alignX, bool drawShadow) {
    FontRenderer* fr = Ethertia::getRenderEngine()->fontRenderer;
    if (drawShadow) {
        float off = textHeight / 10;
        auto scolor = color * 0.24f; scolor.a = color.a;
        fr->renderString(x+off, y+off, str, scolor, textHeight, alignX);
    }
    fr->renderString(x, y, str, color, textHeight, alignX);
}
