//
// Created by Dreamtowards on 2022/4/22.
//

#include <ethertia/event/client/WindowCloseEvent.h>
#include "Window.h"
#include "Ethertia.h"


void Window::onWindowClose(GLFWwindow* _w) {
    WindowCloseEvent e;
    if (EventBus::EVENT_BUS.post(&e))
        return;

    Ethertia::shutdown();
}