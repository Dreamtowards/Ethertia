//
// Created by Dreamtowards on 2022/8/29.
//

#ifndef ETHERTIA_GUIPOPUPMENU_H
#define ETHERTIA_GUIPOPUPMENU_H

#include "GuiCollection.h"
#include "GuiStack.h"
#include "GuiRoot.h"

#include <ethertia/Ethertia.h>

class GuiPopupMenu : public GuiCollection
{
public:
    GuiStack* menulist;

    GuiPopupMenu() {
        setWidth(300);

        menulist = new GuiStack(GuiStack::D_VERTICAL, 3);
        menulist->addDrawBackground(Colors::BLACK50);
        addGui(menulist);

//        EventBus::EVENT_BUS.listen([&](MouseButtonEvent* e) {
//            if (e->isPressed() && isVisible() && !isHover()) {
//                hide();
//            }
//        });
    }

    void show(float x, float y) {
        assert(!getParent());
        setX(x);
        setY(y);
        Ethertia::getRootGUI()->addGui(this);
    }

    void hide() {
        assert(getParent());
        Ethertia::getRootGUI()->removeGui(this);
    }

    void addMenu(Gui* g) {
        menulist->addGui(g);
    }

};

#endif //ETHERTIA_GUIPOPUPMENU_H
