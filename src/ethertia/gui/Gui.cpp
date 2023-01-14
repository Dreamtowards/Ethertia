//
// Created by Dreamtowards on 2022/8/22.
//

#include "Gui.h"

#include <ethertia/render/Window.h>
#include <ethertia/render/renderer/gui/FontRenderer.h>
#include <ethertia/render/RenderEngine.h>

float Gui::maxWidth()  { return Ethertia::getWindow()->getWidth(); }
float Gui::maxHeight() { return Ethertia::getWindow()->getHeight(); }
float Gui::cursorX()   { return Ethertia::getWindow()->getMouseX(); }
float Gui::cursorY()   { return Ethertia::getWindow()->getMouseY(); }

float Gui::mfbWidth()  { return Ethertia::getWindow()->getFramebufferWidth(); }
float Gui::mfbHeight() { return Ethertia::getWindow()->getFramebufferHeight(); }

void Gui::drawRect(float x, float y, float w, float h, Gui::DrawRectArgs args) {
    if (h < 0) {
        h = -h;
        y = y - h;
    }
//    assert(args.tex_pos.x == 0);
//    assert(args.tex_pos.y == 0);
//    assert(args.tex_size.x == 0);
//    assert(args.tex_size.y == 0);
    GuiRenderer* gr = Ethertia::getRenderEngine()->guiRenderer;
    gr->render(x, y, w, h,
               args.color,
               args.tex,
               glm::vec4(args.tex_pos, args.tex_size),
               args.round,
               args.border);
}


void Gui::drawString(float x, float y, const std::string& str, glm::vec4 color, float textHeight, glm::vec2 align, bool drawShadow) {
    FontRenderer* fr = Ethertia::getRenderEngine()->fontRenderer;
    if (align.y != 0) {
        float height = Strings::lines(str) * textHeight;
        y += height * align.y;
    }
    if (drawShadow) {
        float off = textHeight / 16;
        auto scolor = color * 0.24f; scolor.a = color.a;
        fr->renderString(x+off, y+off, str, scolor, textHeight, align.x);
    }
    fr->renderString(x, y, str, color, textHeight, align.x);
}

void Gui::drawWorldpoint(const glm::vec3& worldpos, const std::function<void(glm::vec2)>& fn)
{
    glm::vec3 p = Mth::projectWorldpoint(worldpos, Ethertia::getRenderEngine()->viewMatrix, Ethertia::getRenderEngine()->projectionMatrix);

    p.x = p.x * Gui::maxWidth();
    p.y = p.y * Gui::maxHeight();

    if (p.z > 0) {
        fn(glm::vec2(p.x, p.y));
    }
}

float Gui::toFramebufferCoords(float f) {
    Window* w = Ethertia::getWindow();
    // makesure its same aspectRatio.
    // assert(w->getFramebufferWidth()/Gui::maxWidth() == w->getFramebufferHeight()/Gui::maxHeight());
    return f * (w->getFramebufferWidth() / Gui::maxWidth());
}