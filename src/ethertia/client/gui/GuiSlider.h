//
// Created by Dreamtowards on 2022/8/12.
//

#ifndef ETHERTIA_GUISLIDER_H
#define ETHERTIA_GUISLIDER_H

#include <utility>

#include "Gui.h"
#include "GuiButton.h"

class GuiSlider : public Gui
{
public:
    float percentage = 0.0f;

    std::string label = "Value";
    float min = 0;
    float max = 100;
    float* traceValue = nullptr;

    float unit = 0.1f;

    GuiSlider(std::string label, float min, float max, float* traceVal, float unit = 0.1f) : label(std::move(label)), min(min), max(max),
                                                                          traceValue(traceVal), unit(unit), Gui(0, 0, 140, 20) { }


    void onDraw() override {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();

        GuiButton::drawButtonBackground(this);

        // Handler
        float HANDLER_WIDTH = 10;
        if (isPressed()) {
            percentage = (Gui::cursorX() - x - HANDLER_WIDTH / 2.0f) / (w - HANDLER_WIDTH);
            percentage = Mth::clamp(percentage, 0.0f, 1.0f);
            setValue(getValue());  // round unit.
        }
        Gui::drawRect(x+ (w-HANDLER_WIDTH) * percentage, y, HANDLER_WIDTH, h, Colors::WHITE40);

        // Text
        Gui::drawString(x+w/2, y+(h-16)/2,
                        Strings::fmt("{}: {}", label, getValue()),
                        isPressed() ? Colors::GOLD : isHover() ? Colors::YELLOW : Colors::WHITE,
                        16, 0.5, true);

        // Update trace value
        if (traceValue) {
            float tv = *traceValue;
            float v = getValue();
            if (tv != v) {
                if (isPressed()) {  // Modifying the Slider
                    *traceValue = v;
                } else {
                    setValue(tv);
                }
            }
        }

        Gui::onDraw();
    }

    float getValue() {
        return Mth::lerp(percentage, min, max);
    }
    void setValue(float val) {
        percentage = Mth::rlerp(Mth::round(val, unit), min, max);
    }

};

#endif //ETHERTIA_GUISLIDER_H
