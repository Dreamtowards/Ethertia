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

void Gui::drawRect(Gui::DrawRectArgs args) {
    glm::vec4 xywh = args.xywh;
    DECL_XYWH;
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
               args.border,
               args.channel_mode);
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
    glm::vec3 p = Mth::projectWorldpoint(worldpos, RenderEngine::matView, RenderEngine::matProjection);

    p.x = p.x * Gui::maxWidth();
    p.y = p.y * Gui::maxHeight();

    if (p.z > 0) {
        fn(glm::vec2(p.x, p.y));
    }
}

void Gui::drawStretchCorners(glm::vec4 xywh, Texture* tex, float thickness, bool onlyLTCornerTex) {
    float x = xywh.x, y = xywh.y, w = xywh.z, h = xywh.w;

    float tk = thickness;
    float iw = w-tk-tk;  // inner width
    float ih = h-tk-tk;

    if (onlyLTCornerTex)
    {
        // 4 Corners
        Gui::drawRect(x,y,tk,tk, {.tex=tex, .tex_pos={0,1.0}, .tex_size={1.0,1.0}});  // LT
        Gui::drawRect(x+w-tk,y,tk,tk, {.tex=tex, .tex_pos={1.0,1.0}, .tex_size={-1.0,1.0}});  // RT
        Gui::drawRect(x,y+h-tk,tk,tk, {.tex=tex, .tex_pos={0,0}, .tex_size={1.0,-1.0}});  // LB
        Gui::drawRect(x+w-tk,y+h-tk,tk,tk, {.tex=tex, .tex_pos={1.0, 0}, .tex_size={-1.0,-1.0}});  // RB

        // Center
        Gui::drawRect(x+tk,y+tk,iw,ih, {.tex=tex, .tex_pos={0.99,0.01}, .tex_size={0,0}});  // RB

        // 4 Borders
        Gui::drawRect(x,y+tk,tk,ih, {.tex=tex, .tex_pos={0,0}, .tex_size={1.0,0}});  // L
        Gui::drawRect(x+w-tk,y+tk,tk,ih, {.tex=tex, .tex_pos={0,0}, .tex_size={-0.99,0}});  // R
        Gui::drawRect(x+tk,y,iw,tk, {.tex=tex, .tex_pos={0.99,1.0}, .tex_size={0,1.0}});  // T
        Gui::drawRect(x+tk,y+h-tk,iw,tk, {.tex=tex, .tex_pos={0.99,0}, .tex_size={0,-1.0}});  // B
    }
    else
    {
        // 4 Corners
        Gui::drawRect(x,y,tk,tk, {.tex=tex, .tex_pos={0,0.5}, .tex_size={0.5,0.5}});  // LT
        Gui::drawRect(x+w-tk,y,tk,tk, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0.5,0.5}});  // RT
        Gui::drawRect(x,y+h-tk,tk,tk, {.tex=tex, .tex_pos={0,0}, .tex_size={0.5,0.5}});  // LB
        Gui::drawRect(x+w-tk,y+h-tk,tk,tk, {.tex=tex, .tex_pos={0.5, 0}, .tex_size={0.5,0.5}});  // RB

        // Center
        Gui::drawRect(x+tk,y+tk,iw,ih, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0,0}});  // RB

        // 4 Borders
        Gui::drawRect(x,y+tk,tk,ih, {.tex=tex, .tex_pos={0,0.5}, .tex_size={0.5,0}});  // L
        Gui::drawRect(x+w-tk,y+tk,tk,ih, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0.5,0}});  // R
        Gui::drawRect(x+tk,y,iw,tk, {.tex=tex, .tex_pos={0.5,0.5}, .tex_size={0,0.5}});  // T
        Gui::drawRect(x+tk,y+h-tk,iw,tk, {.tex=tex, .tex_pos={0.5,0}, .tex_size={0,0.5}});  // B
    }
}

float Gui::toFramebufferCoords(float f) {
    Window* w = Ethertia::getWindow();
    // makesure its same aspectRatio.
    // assert(w->getFramebufferWidth()/Gui::maxWidth() == w->getFramebufferHeight()/Gui::maxHeight());
    return f * (w->getFramebufferWidth() / Gui::maxWidth());
}