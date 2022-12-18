//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_GUIROOT_H
#define ETHERTIA_GUIROOT_H

#include "Gui.h"
#include "GuiCollection.h"

#include <ethertia/event/client/MouseButtonEvent.h>
#include <ethertia/event/client/WindowCloseEvent.h>

class GuiRoot : public GuiCollection
{


public:
    GuiRoot(int _w, int _h) {
        setWidth(_w);
        setHeight(_h);
//        setWidth(Ethertia::getWindow()->getWidth());
//        setHeight(Ethertia::getWindow()->getHeight());

        EventBus::EVENT_BUS.listen([](WindowResizedEvent* e) {
//            setWidth();
        });
    }

    void onMouseButton(int button, bool pressed) override
    {
        if (pressed) {  // mouse release should not effect the focus stat
            updateFocused();
        }

        updatePressed(pressed);
    }

    /**
     * call every frame. not just when mouse move.
     * because hovered status might change regardless mouse move, e.g. gui pos/size change.
     */
    void updateHovers(glm::vec2 mpos) {
        Gui* innermost = Gui::pointing(this, mpos);

        std::vector<Gui*> hovered;
        Gui::forParents(innermost, [&hovered](Gui *g) {
            g->setHover(true);
            hovered.push_back(g);
        });

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
