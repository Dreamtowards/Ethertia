//
// Created by Dreamtowards on 2022/3/31.
//

#ifndef ETHERTIA_WINDOW_H
#define ETHERTIA_WINDOW_H

#include <stdexcept>
#include "glad/glad.h"
#include "GLFW/glfw3.h"


class Window
{
    GLFWwindow* window;

    float mouseX;
    float mouseY;
    float mouseDX;
    float mouseDY;
    float scrollDX;
    float scrollDY;

    float width;
    float height;

public:

    int initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // OSX Required.

        window = glfwCreateWindow(width=600, height=420, "Dysplay", nullptr, nullptr);
        if (!window)
            throw std::runtime_error("Failed to init GLFW window.");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to init GLAD.");

        glfwSetWindowUserPointer(window, this);
        glfwSetWindowCloseCallback(window, onWindowClose);
        glfwSetCursorPosCallback(window, onCursorPos);
        glfwSetWindowSizeCallback(window, onWindowSize);

        return 0;
    }

    void updateWindow()
    {
        resetDeltas();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    bool isCloseRequested() {
        return glfwWindowShouldClose(window);
    }

    static double getPreciseTime() {
        return glfwGetTime();
    }

    float getMouseX() { return mouseX; }
    float getMouseY() { return mouseY; }
    float getMouseDX() { return mouseDX; }
    float getMouseDY() { return mouseDY; }
    float getScrollDX() { return scrollDX; }
    float getScrollDY() { return scrollDY; }
    float getDScroll() { return scrollDX+scrollDY; }

    float getWidth() { return width; }
    float getHeight() { return height; }

    void setTitle(const char* s) {
        glfwSetWindowTitle(window, s);
    }

    bool isKeyDown(int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }
    bool isMouseDown(int button) {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    void setMouseGrabbed(bool grabbed) {
        glfwSetInputMode(window, GLFW_CURSOR, grabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    bool isShiftKeyDown() {
        return isKeyDown(GLFW_KEY_LEFT_SHIFT) || isKeyDown(GLFW_KEY_RIGHT_SHIFT);
    }
    bool isAltKeyDown() {
        return isKeyDown(GLFW_KEY_LEFT_ALT) || isKeyDown(GLFW_KEY_RIGHT_ALT);
    }

    void resetDeltas() {
        mouseDX = 0;
        mouseDY = 0;
        scrollDX = 0;
        scrollDY = 0;
    }

    static void onWindowClose(GLFWwindow* _w);

    static void onCursorPos(GLFWwindow* _w, double xpos, double ypos) {
        Window* w = (Window*)glfwGetWindowUserPointer(_w);
        float x = (float)xpos;
        float y = (float)ypos;
        w->mouseDX = x - w->mouseX;
        w->mouseDY = y - w->mouseY;
        w->mouseX = x;
        w->mouseY = y;
    }

    static void onWindowSize(GLFWwindow* _w, int wid, int hei) {
        Window* w = (Window*)glfwGetWindowUserPointer(_w);
        w->width = wid;
        w->height = hei;
    }

};

#endif //ETHERTIA_WINDOW_H
