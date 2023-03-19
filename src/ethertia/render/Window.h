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
#include <ethertia/util/BitmapImage.h>
#include <ethertia/event/window/KeyboardEvent.h>
#include <ethertia/event/window/CharInputEvent.h>
#include <ethertia/event/window/MouseButtonEvent.h>
#include <ethertia/event/window/MouseMoveEvent.h>
#include <ethertia/event/window/MouseScrollEvent.h>
#include <ethertia/event/window/WindowResizedEvent.h>
#include <ethertia/event/window/WindowDropEvent.h>
#include <ethertia/event/window/WindowFocusEvent.h>
#include <ethertia/event/window/WindowCloseEvent.h>


class Window
{

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

    bool m_Fullscreen = false;

    EventBus m_Eventbus;

public:
    GLFWwindow* m_WindowHandle = nullptr;

    bool m_MouseGrabbed = false;

    Window(int _w, int _h, const char* _title);

    EventBus& eventbus() {
        return m_Eventbus;
    }

    void swapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    bool isCloseRequested() {
        return glfwWindowShouldClose(m_WindowHandle);
    }

//    void setWindowSize(int _w, int _h) {
//        glfwSetWindowSize(window, _w, _h);
//    }

    void fullscreen(GLFWmonitor* monitor = glfwGetPrimaryMonitor()) {
        m_Fullscreen = true;

        const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_WindowHandle, monitor, 0, 0, vmode->width, vmode->height, 0);
    }

    void restoreFullscreen(int _w = 1280, int _h = 720) {
        m_Fullscreen = false;

        glfwSetWindowMonitor(m_WindowHandle, nullptr, 0, 0, _w, _h, 0);
        centralize();
    }

    bool isFullscreen() {
        return m_Fullscreen;
    }

    void toggleFullscreen() {
        if (m_Fullscreen) {
            restoreFullscreen();
        } else {
            fullscreen();
        }
    }

    void centralize() {
        const GLFWvidmode* vmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int w, h;
        glfwGetWindowSize(m_WindowHandle, &w, &h);
        glfwSetWindowPos(m_WindowHandle, (vmode->width - w) / 2, (vmode->height - h) / 2);
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
        glfwSetWindowTitle(m_WindowHandle, s);
    }

    bool isKeyDown(int key) {
        return glfwGetKey(m_WindowHandle, key) == GLFW_PRESS;
    }
    bool isMouseDown(int button) {
        return glfwGetMouseButton(m_WindowHandle, button) == GLFW_PRESS;
    }
    bool isMouseLeftDown() {
        return isMouseDown(GLFW_MOUSE_BUTTON_LEFT);
    }
    bool isMouseRightDown() {
        return isMouseDown(GLFW_MOUSE_BUTTON_RIGHT);
    }
    bool isMouseMiddleDown() {
        return isMouseDown(GLFW_MOUSE_BUTTON_MIDDLE);
    }

    const char* getClipboard() {
        return glfwGetClipboardString(m_WindowHandle);
    }
    void setClipboard(const char* str) {
        glfwSetClipboardString(m_WindowHandle, str);
    }

    bool m_GrabbedChanged = false;
    void setMouseGrabbed(bool grabbed) {
//        static double lastEnableX = 0, lastEnableY = 0;
        if (m_MouseGrabbed != grabbed) {
            m_MouseGrabbed = grabbed;
            m_GrabbedChanged = true;

//            if (grabbed) {
//                glfwGetCursorPos(m_WindowHandle, &lastEnableX, &lastEnableY);
//            }

            glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, m_MouseGrabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

//            if (!grabbed) {
//                glfwSetCursorPos(m_WindowHandle, lastEnableX, lastEnableY);
//            }
        }
    }
    void setStickyKeys(bool s) {
        glfwSetInputMode(m_WindowHandle, GLFW_STICKY_KEYS, s ? GLFW_TRUE : GLFW_FALSE);
    }

    void setMousePos(float x, float y) {
        glfwSetCursorPos(m_WindowHandle, x, y);
    }


    void maximize() {
        glfwMaximizeWindow(m_WindowHandle);
    }
    void restoreMaximize() {
        glfwRestoreWindow(m_WindowHandle);
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

    void setVSync(bool vsync) {
        glfwSwapInterval(vsync ? 1 : 0);
    }

    static void onWindowClose(GLFWwindow* _win) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);

        WindowCloseEvent e;
        win->eventbus().post(&e);
    }

    static void onFramebufferSize(GLFWwindow* _w, int wid, int hei) {
        Window* _win = (Window*)glfwGetWindowUserPointer(_w);
        _win->framebufferWidth  = wid;
        _win->framebufferHeight = hei;

        glViewport(0, 0, wid, hei);
    }

    static void onWindowSize(GLFWwindow* _win, int wid, int hei) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);
        win->width  = wid;
        win->height = hei;

        WindowResizedEvent e(wid, hei);
        win->eventbus().post(&e);
    }

    static void onWindowDropPath(GLFWwindow* _win, int count, const char** paths) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);

        WindowDropEvent e(count, paths);
        win->eventbus().post(&e);
    }

    static void onWindowFocus(GLFWwindow* _win, int focused) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);

        WindowFocusEvent e;
        win->eventbus().post(&e);
    }

    // CNS: 这里有2个不准确的 DX，就是在切换 glfwSetInputMode(...DISABLE/NORMAL) 的时候
    // 当case 1. 禁用变成启用后时 鼠标位置突然从 禁用时的最后位置(启用前的位置) 变成了上次最后启用时的位置 这是由于内部状态特例改变 而造成的没有意义的DX
    //           但由于这种情况 通常只在禁用时检测DX 所以启用后产生的巨大DX 通常影响不到 因为自从启用 就已经不检测DX了
    // 当case 2. 启用变成禁用后 内部会设置状态 产生巨大DX 由于禁用时会监听DX 所以会Bang的一下一个巨大DX 由最后启用状态位置 变成最后禁用状态位置的DX
    //
    static void onCursorPos(GLFWwindow* _win, double xpos, double ypos) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);
        float x = (float)xpos;
        float y = (float)ypos;
        // for fix bug:
        // Extreme DX caused by glfwSetInputMode(GLFW_CURSOR_DISABLE/NORMAL), glfw internal will set CursorPos when
        // switching Cursor's Disable/Normal. so we just ignore the first DX after that switch.
        if (win->m_GrabbedChanged) {
            win->m_GrabbedChanged = false;
            win->mouseX = x;
            win->mouseY = y;
            return;
        }
        win->mouseDX = x - win->mouseX;
        win->mouseDY = y - win->mouseY;
        win->mouseX = x;
        win->mouseY = y;

        MouseMoveEvent e;
        win->eventbus().post(&e);
    }

    static void onMouseButton(GLFWwindow* _win, int button, int action, int mods) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);

        if (action != GLFW_PRESS && action != GLFW_RELEASE) {
            Log::warn("Unknown mouse button action ", action);  // REPEAT?
            return;
        }

        MouseButtonEvent e(button, action);
        win->eventbus().post(&e);
    }

    static void onScroll(GLFWwindow* _win, double xoffset, double yoffset) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);
        win->scrollDX = (float)xoffset;
        win->scrollDY = (float)yoffset;

        MouseScrollEvent e(xoffset, yoffset);
        win->eventbus().post(&e);
    }

    static void onKeyboardKey(GLFWwindow* _win, int key, int scancode, int action, int mods) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);

        KeyboardEvent e(key, action==GLFW_PRESS );
        win->eventbus().post(&e);
    }

    static void onCharInput(GLFWwindow* _win, unsigned int codepoint) {
        Window* win = (Window*)glfwGetWindowUserPointer(_win);

        CharInputEvent e(codepoint);
        win->eventbus().post(&e);
    }


    BitmapImage* screenshot()
    {
        BitmapImage* img = new BitmapImage(framebufferWidth, framebufferHeight);

        glReadPixels(0, 0, img->getWidth(), img->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, img->getPixels());

        img->fillAlpha(1.0);

        return img;
    }
};

#endif //ETHERTIA_WINDOW_H
