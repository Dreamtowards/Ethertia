//
// Created by Dreamtowards on 2022/4/22.
//

#include "Window.h"
#include "Ethertia.h"


void Window::onWindowClose(GLFWwindow* _w) {
    Ethertia::shutdown();
}