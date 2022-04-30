//
// Created by Dreamtowards on 2022/3/31.
//

#ifndef ETHERTIA_WINDOW_H
#define ETHERTIA_WINDOW_H

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ethertia/util/Log.h>
#include <ethertia/event/EventBus.h>
#include <ethertia/event/client/MouseButtonEvent.h>
#include <ethertia/event/client/KeyboardEvent.h>
#include <ethertia/event/client/MouseMoveEvent.h>
#include <ethertia/event/client/WindowResizeEvent.h>
#include <ethertia/event/client/CharInputEvent.h>
#include <ethertia/event/client/WindowDropEvent.h>
#include <ethertia/event/client/WindowFocusEvent.h>
#include <ethertia/event/client/MouseScrollEvent.h>


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

        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

        window = glfwCreateWindow(width=600, height=420, "Dysplay", nullptr, nullptr);
        if (!window)
            throw std::runtime_error("Failed to init GLFW window.");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to init GLAD.");

        Log::info("WindowInit. GLFW {}; GLAD linked.", glfwGetVersionString());

        centralize();

        glfwSetWindowUserPointer(window, this);

        glfwSetWindowCloseCallback(window, onWindowClose);
        glfwSetWindowSizeCallback(window, onWindowSize);
        glfwSetDropCallback(window, onWindowDropPath);
        glfwSetWindowFocusCallback(window, onWindowFocus);

        glfwSetCursorPosCallback(window, onCursorPos);
        glfwSetMouseButtonCallback(window, onMouseButton);
        glfwSetScrollCallback(window, onScroll);
        glfwSetKeyCallback(window, onKeyboardKey);
        glfwSetCharCallback(window, onCharInput);

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

    void centralize() {
        const GLFWvidmode* vmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        glfwSetWindowPos(window, (vmode->width - w) / 2, (vmode->height - h) / 2);
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

    const char* getClipboard() {
        return glfwGetClipboardString(window);
    }
    void setClipboard(const char* str) {
        glfwSetClipboardString(window, str);
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

    static void onWindowSize(GLFWwindow* _w, int wid, int hei) {
        Window* w = (Window*)glfwGetWindowUserPointer(_w);
        w->width = wid;
        w->height = hei;

        WindowResizeEvent e;
        EventBus::EVENT_BUS.post(&e);
    }

    static void onWindowDropPath(GLFWwindow* _w, int count, const char** paths) {

        WindowDropEvent e(count, paths);
        EventBus::EVENT_BUS.post(&e);
    }

    static void onWindowFocus(GLFWwindow* _w, int focused) {

        WindowFocusEvent e;
        EventBus::EVENT_BUS.post(&e);
    }

    static void onCursorPos(GLFWwindow* _w, double xpos, double ypos) {
        Window* w = (Window*)glfwGetWindowUserPointer(_w);
        float x = (float)xpos;
        float y = (float)ypos;
        w->mouseDX = x - w->mouseX;
        w->mouseDY = y - w->mouseY;
        w->mouseX = x;
        w->mouseY = y;

        MouseMoveEvent e;
        EventBus::EVENT_BUS.post(&e);
    }

    static void onMouseButton(GLFWwindow* _w, int button, int action, int mods) {

        MouseButtonEvent e;
        EventBus::EVENT_BUS.post(&e);
    }

    static void onScroll(GLFWwindow* _w, double xoffset, double yoffset) {
        Window* w = (Window*)glfwGetWindowUserPointer(_w);
        w->scrollDX = (float)xoffset;
        w->scrollDY = (float)yoffset;

        MouseScrollEvent e;
        EventBus::EVENT_BUS.post(&e);
    }

    static void onKeyboardKey(GLFWwindow* _w, int key, int scancode, int action, int mods) {

        KeyboardEvent e;
        EventBus::EVENT_BUS.post(&e);
    }

    static void onCharInput(GLFWwindow* _w, unsigned int codepoint) {

        CharInputEvent e;
        EventBus::EVENT_BUS.post(&e);
    }
};

#endif //ETHERTIA_WINDOW_H
