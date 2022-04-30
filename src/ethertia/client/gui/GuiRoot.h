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
public:
    GuiRoot() {

        Log::info("Registered MouseButton Lsr");
        EventBus::EVENT_BUS.listen(&GuiRoot::onMouseButton, this);

        EventBus::EVENT_BUS.listen([](WindowDropEvent* e) {
            Log::info("Window Drop Paths: {}", e->count());
            for (int i = 0; i < e->count(); ++i) {
                Log::info("Path {}: {}", i, e->path(i));
            }
        });

        EventBus::EVENT_BUS.listen([](WindowCloseEvent* e) {
            static int i = 0;
            Log::info("Window Close: ", i);
            if (i++ < 10)
                throw EventBus::FORCE_CANCEL;
        });
    }

    void onMouseButton(MouseButtonEvent* e) {

        Log::info("MouseButton Fired");
    }
};

#endif //ETHERTIA_GUIROOT_H
