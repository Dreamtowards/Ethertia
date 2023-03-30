//
// Created by Dreamtowards on 2022/8/22.
//

#ifndef ETHERTIA_GUISCROLLBOX_H
#define ETHERTIA_GUISCROLLBOX_H

#include "GuiDelegate.h"

class GuiScrollBox : public GuiDelegate
{
public:

    inline static float HANDLE_THICKNESS = 4;

    float scrollY = 0;

    bool draggingScrollY = false;

    void onDraw() override {
        float x = getX();
        float y = getY();
        float h = getHeight();
        float w = getWidth();

        Gui* g = getContent();
        float g_h = g->getHeight();
        float g_w = g->getWidth();

        {
            // Vertical Handler
            float vhSize = h / g_h;
            if (vhSize < 1.0) {
                float hx = x+w-HANDLE_THICKNESS;
                float hh = vhSize*h;  // handler height
                Gui::drawRect(hx, y, HANDLE_THICKNESS, h, Colors::BLACK60);

                Gui::drawRect(hx,y+(-scrollY/g_h*h), HANDLE_THICKNESS,hh, Colors::WHITE40);


                if (isPressed()) {
                    if (Gui::isCursorOver(hx, y, HANDLE_THICKNESS, h)) {
                        draggingScrollY = true;
                    }
                    if (draggingScrollY) {
                        scrollY = - ((Gui::cursorY() - hh/2.0f - y) / h) * g_h;
                    }
                }
            }
        }

        if (isHover()) {
            scrollY += Ethertia::getWindow()->getScrollDY();

        }
        scrollY = Mth::clamp(scrollY, -(g_h-h), 0.0f);

        g->setRelativeY(scrollY);


        Gui::gPushScissor(x,y,w,h);
        Gui::onDraw();
        Gui::gPopScissor();

        if (!isPressed()) {
            // clear states.
            draggingScrollY = false;
        }
    }


};

#endif //ETHERTIA_GUISCROLLBOX_H
