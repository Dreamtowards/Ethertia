//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_GUIROOT_H
#define ETHERTIA_GUIROOT_H

#include "Gui.h"
#include <ethertia/event/client/MouseButtonEvent.h>
#include <ethertia/event/client/WindowCloseEvent.h>

class GuiRoot : public Gui
{
    void onMouseButton(MouseButtonEvent* e)
    {

        updateFocused();

        updatePressed(e->isPressed());
    }

    void onWindowResized(WindowResizedEvent* e) {

        setWidth(e->getWidth());
        setHeight(e->getHeight());
    }

public:
    GuiRoot() {

        EventBus::EVENT_BUS.listen(&GuiRoot::onMouseButton, this);
        EventBus::EVENT_BUS.listen(&GuiRoot::onWindowResized, this);
    }

    /**
     * call every frame. not just when mouse move.
     * because hovered status might change regardless mouse move, e.g. gui pos/size change.
     */
    void updateHovers(glm::vec2 mpos) {
        Gui* innermost = Gui::pointing(this, mpos);

        std::vector<Gui*> hovered;
        if (innermost) {
            Gui::forParents(innermost, [&hovered](Gui *g) {
                g->setHover(true);
                hovered.push_back(g);
            });
        }

        Gui::forChildren(this, [&hovered](Gui* g) {
            if (g->isHover() && !Collections::contains(hovered, g)) {
                g->setHover(false);
            }
        });
    }

    /**
     * call only when MouseButton Down.
     */
    void updateFocused() {

        Gui::forChildren(this, [](Gui* g) {
            g->setFocused(g->isHover());
        });
    }

    /**
     * call only when MouseButton Down/Up.
     */
    void updatePressed(bool pressed)
    {
        if (pressed) {
            Gui::forChildren(this, [](Gui* g) {
                if (g->isHover())
                    g->setPressed(true);
            });
        } else {
            Gui::forChildren(this, [](Gui* g) {
                if (g->isPressed())
                    g->setPressed(false);
            });
        }
    }

};

#endif //ETHERTIA_GUIROOT_H
