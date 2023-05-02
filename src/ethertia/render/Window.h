//
// Created by Dreamtowards on 2022/3/31.
//

#ifndef ETHERTIA_WINDOW_H
#define ETHERTIA_WINDOW_H

#include <stdexcept>
#define GLFW_INCLUDE_NONE 1
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <ethertia/util/Log.h>
#include <ethertia/event/EventBus.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/BitmapImage.h>



class Window
{

public:
    GLFWwindow* m_WindowHandle = nullptr;

    float m_MouseX = 0;
    float m_MouseY = 0;
    float m_MouseDX = 0;
    float m_MouseDY = 0;
    float m_ScrollDX = 0;
    float m_ScrollDY = 0;

    int m_Width  = 0;  // 600, 420
    int m_Height = 0;

    // WindowCoord * ContentScale = FramebufferSize.
    int m_FramebufferWidth  = 0;
    int m_FramebufferHeight = 0;

    bool m_IsFramebufferResized = false;

    bool m_Fullscreen = false;

    EventBus m_Eventbus;

    bool m_MouseGrabbed = false;



    Window(int _w, int _h, const char* _title);

    EventBus& eventbus() {
        return m_Eventbus;
    }

    void swapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }

    bool isCloseRequested();

    bool isFramebufferResized();

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

    float getMouseX() const { return m_MouseX; }
    float getMouseY() const { return m_MouseY; }
    float getMouseDX() const { return m_MouseDX; }
    float getMouseDY() const { return m_MouseDY; }
    float getScrollDX() const { return m_ScrollDX; }
    float getScrollDY() const { return m_ScrollDY; }
    float getDScroll() const { return getScrollDX()+getScrollDY(); }

    glm::vec2 getMousePos() const { return glm::vec2(m_MouseX, m_MouseY); }

    int getWidth()  const { return m_Width; }
    int getHeight() const { return m_Height; }
    int getFramebufferWidth()  const { return m_FramebufferWidth;  }
    int getFramebufferHeight() const { return m_FramebufferHeight; }


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
    void setMouseGrabbed(bool grabbed)
    {
        if (m_MouseGrabbed != grabbed)
        {
            m_MouseGrabbed = grabbed;
            m_GrabbedChanged = true;

            glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, m_MouseGrabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
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

    void resetFrameStates() {
        m_MouseDX = 0;
        m_MouseDY = 0;
        m_ScrollDX = 0;
        m_ScrollDY = 0;

        m_IsFramebufferResized = false;
    }

    void PollEvents()
    {
        resetFrameStates();
        glfwPollEvents();
    }

    void setVSync(bool vsync) {
        glfwSwapInterval(vsync ? 1 : 0);
    }

    BitmapImage* screenshot();
};

#endif //ETHERTIA_WINDOW_H
