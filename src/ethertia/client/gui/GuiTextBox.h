//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUITEXTBOX_H
#define ETHERTIA_GUITEXTBOX_H

#include "Gui.h"

class GuiTextBox : public Gui
{
    std::string text;

public:
    void setText(const std::string& t) {
        if (t == text) return;
        text = t;
    }

    std::string getText() const {
        return text;
    }

};

#endif //ETHERTIA_GUITEXTBOX_H
