//
// Created by Dreamtowards on 2022/8/29.
//

#ifndef ETHERTIA_GUICHECKBOX_H
#define ETHERTIA_GUICHECKBOX_H

#include "Gui.h"

class GuiCheckBox : public Gui
{
public:

    std::string text;

    bool checked = false;

    bool* traceValue = nullptr;

    GuiCheckBox(std::string text, bool* trace = nullptr) : text(std::move(text)), traceValue(trace), Gui(0, 0, 100, 20) {

        addOnClickListener([&](OnReleased* e) {
            checked = !checked;

            if (traceValue) {
                *traceValue = checked;
            }
        });
    }


    void onDraw() override
    {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();

        Gui::drawString(x + 22, y + 2, text,
                        isPressed() ? Colors::GOLD : isHover() ? Colors::YELLOW : Colors::WHITE);

        Gui::drawRect(x+2, y+2, 16, 16, Colors::BLACK30);


        if (traceValue && *traceValue != checked) {
            checked = *traceValue;
        }

        if (checked || isHover()) {
            Gui::drawRect(x+4, y+4, 12, 12, checked ? Colors::WHITE40 : Colors::WHITE20);
        }
    }



};

#endif //ETHERTIA_GUICHECKBOX_H
