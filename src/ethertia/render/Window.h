//
// Created by Dreamtowards on 2022/3/31.
//

#ifndef ETHERTIA_WINDOW_H
#define ETHERTIA_WINDOW_H

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <ethertia/util/Log.h>
#include <ethertia/event/EventBus.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/event/client/KeyboardEvent.h>
#include <ethertia/event/client/CharInputEvent.h>
#include <ethertia/event/client/MouseButtonEvent.h>
#include <ethertia/event/client/MouseMoveEvent.h>
#include <ethertia/event/client/MouseScrollEvent.h>
#include <ethertia/event/client/WindowResizedEvent.h>
#include <ethertia/event/client/WindowDropEvent.h>
#include <ethertia/event/client/WindowFocusEvent.h>
#include <ethertia/event/client/WindowCloseEvent.h>


class Window
{
    GLFWwindow* window = nullptr;

    float mouseX = 0;
    float mouseY = 0;
    float mouseDX = 0;
    float mouseDY = 0;
    float scrollDX = 0;
    float scrollDY = 0;

    int width  = 0;  // 600, 420
    int height = 0;

    // WindowCoord * ContentScale = FramebufferSize.
    int framebufferWidth  = 0;
    int framebufferHeight = 0;

    EventBus m_Eventbus;

public:

    Window(int _w, int _h, const char* _title) : width(_w), height(_h)
    {
        BenchmarkTimer _tm;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // OSX Required.
        // glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

        window = glfwCreateWindow(width, height, _title, nullptr, nullptr);
        if (!window) {
            const char* err_str;
            int err = glfwGetError(&err_str);
            throw std::runtime_error(Strings::fmt("Failed to init GLFW window. Err {}. ({})", err, err_str));
        }

        centralize();

        // fireWindowResize(). init very first events. e.g. init GuiRoot's size.
        // onWindowSize(window, width, height);

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to init GLAD.");

        Log::info("Init GL_{}, {} | GLFW {}.\1",
                  glGetString(GL_VERSION), glGetString(GL_RENDERER), //glGetString(GL_VENDOR),
                  glfwGetVersionString());

        glfwSetWindowUserPointer(window, this);

        glfwSetWindowCloseCallback(window, onWindowClose);
        glfwSetWindowSizeCallback(window, onWindowSize);
        glfwSetFramebufferSizeCallback(window, onFramebufferSize);
        glfwSetDropCallback(window, onWindowDropPath);
        glfwSetWindowFocusCallback(window, onWindowFocus);

        glfwSetCursorPosCallback(window, onCursorPos);
        glfwSetMouseButtonCallback(window, onMouseButton);
        glfwSetScrollCallback(window, onScroll);
        glfwSetKeyCallback(window, onKeyboardKey);
        glfwSetCharCallback(window, onCharInput);

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    }

    EventBus& eventbus() {
        return m_Eventbus;
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

//    void setSize(int _w, int _h) {
//        glfwSetWindowSize(window, _w, _h);
//    }

    void centralize() {
        const GLFWvidmode* vmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        glfwSetWindowPos(window, (vmode->width - w) / 2, (vmode->height - h) / 2);
    }

    static double getPreciseTime() {
        return glfwGetTime();
    }

    float getMouseX() const { return mouseX; }
    float getMouseY() const { return mouseY; }
    float getMouseDX() const { return mouseDX; }
    float getMouseDY() const { return mouseDY; }
    float getScrollDX() const { return scrollDX; }
    float getScrollDY() const { return scrollDY; }
    float getDScroll() const { return scrollDX+scrollDY; }

    glm::vec2 getMousePos() const { return glm::vec2(mouseX, mouseY); }

    int getWidth()  const { return width; }
    int getHeight() const { return height; }
    int getFramebufferWidth()  const { return framebufferWidth;  }
    int getFramebufferHeight() const { return framebufferHeight; }


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
    void setMousePos(float x, float y) {
        glfwSetCursorPos(window, x, y);
    }

    void setStickyKeys(bool s) {
        glfwSetInputMode(window, GLFW_STICKY_KEYS, s ? GLFW_TRUE : GLFW_FALSE);
    }

    bool isShiftKeyDown() {
        return isKeyDown(GLFW_KEY_LEFT_SHIFT) || isKeyDown(GLFW_KEY_RIGHT_SHIFT);
    }
    bool isAltKeyDown() {
        return isKeyDown(GLFW_KEY_LEFT_ALT) || isKeyDown(GLFW_KEY_RIGHT_ALT);
    }
    bool isCtrlKeyDown() {
        return isKeyDown(GLFW_KEY_LEFT_CONTROL) || isKeyDown(GLFW_KEY_RIGHT_CONTROL);
    }

    void resetDeltas() {
        mouseDX = 0;
        mouseDY = 0;
        scrollDX = 0;
        scrollDY = 0;
    }

    static void onWindowClose(GLFWwindow* _w) {
        WindowCloseEvent e;
        if (EventBus::EVENT_BUS.post(&e))
            return;

        Ethertia::shutdown();
    }

    static void onFramebufferSize(GLFWwindow* _w, int wid, int hei) {
        Window* _win = (Window*)glfwGetWindowUserPointer(_w);
        _win->framebufferWidth  = wid;
        _win->framebufferHeight = hei;

        glViewport(0, 0, wid, hei);
    }

    static void onWindowSize(GLFWwindow* _w, int wid, int hei) {
        Window* _win = (Window*)glfwGetWindowUserPointer(_w);
        _win->width  = wid;
        _win->height = hei;

        WindowResizedEvent e(wid, hei);
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

        MouseButtonEvent e(button, action==GLFW_PRESS);
        EventBus::EVENT_BUS.post(&e);
    }

    static void onScroll(GLFWwindow* _w, double xoffset, double yoffset) {
        Window* w = (Window*)glfwGetWindowUserPointer(_w);
        w->scrollDX = (float)xoffset;
        w->scrollDY = (float)yoffset;

        MouseScrollEvent e(xoffset, yoffset);
        EventBus::EVENT_BUS.post(&e);
    }

    static void onKeyboardKey(GLFWwindow* _w, int key, int scancode, int action, int mods) {

        KeyboardEvent e(key, action==GLFW_PRESS);
        EventBus::EVENT_BUS.post(&e);
    }

    static void onCharInput(GLFWwindow* _w, unsigned int codepoint) {

        CharInputEvent e(codepoint);
        EventBus::EVENT_BUS.post(&e);
    }
};

#endif //ETHERTIA_WINDOW_H
