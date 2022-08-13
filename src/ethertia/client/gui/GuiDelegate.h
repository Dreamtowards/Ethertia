//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIDELEGATE_H
#define ETHERTIA_GUIDELEGATE_H

#include "Gui.h"

class GuiDelegate : public Gui
{
    Gui* content = nullptr;

public:
    GuiDelegate(Gui* content) : content(content) {}

    GuiDelegate() {}

    int count() const override {
        return content ? 1 : 0;
    }

    Gui* at(int i) override {
        assert(i == 0 && content);
        return getContent();
    }

    Gui* getContent() {
        return content;
    }

    Gui* setContent(Gui* g) {
        if (content) { content->detach(); }
        content = g;
        if (content) { content->attach(this); }
        return this;
    }

};

#endif //ETHERTIA_GUIDELEGATE_H
