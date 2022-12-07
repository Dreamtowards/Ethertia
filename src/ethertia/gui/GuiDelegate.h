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
        if (content) { content->setParent(nullptr); }
        content = g;
        if (content) { content->setParent(this); }
        return this;
    }




    void onKeyboard(int key, bool pressed) override {
        if (content && content->isVisible()) {
            content->onKeyboard(key, pressed);
        }
    }

    void onMouseButton(int button, bool pressed) override {

        if (content && content->isVisible()) {
            content->onMouseButton(button, pressed);
        }

    }

    void onCharInput(int ch) override {
        if (content && content->isVisible()) {
            content->onCharInput(ch);
        }
    }

    void onScroll(float dx, float dy) override {
        if (content && content->isVisible()) {
            content->onScroll(dx, dy);
        }
    }
};

#endif //ETHERTIA_GUIDELEGATE_H
