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


//#define GL
#define VULKAN


class Window
{
public:
    static void init();
    static void deinit();
    static double getPreciseTime();

    Window(int _w, int _h, const char* _title);
    ~Window();

    bool isCloseRequested();
    bool isFramebufferResized();

    void centralize();

    void fullscreen(GLFWmonitor* monitor = glfwGetPrimaryMonitor());
    void restoreFullscreen(int _w = 1280, int _h = 720);
    bool isFullscreen() const;
    void toggleFullscreen();


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

    bool isKeyDown(int key) { return glfwGetKey(m_WindowHandle, key) == GLFW_PRESS; }
    bool isMouseDown(int button) { return glfwGetMouseButton(m_WindowHandle, button) == GLFW_PRESS; }

    bool isMouseLeftDown() { return isMouseDown(GLFW_MOUSE_BUTTON_LEFT); }
    bool isMouseRightDown() { return isMouseDown(GLFW_MOUSE_BUTTON_RIGHT); }
    bool isMouseMiddleDown() { return isMouseDown(GLFW_MOUSE_BUTTON_MIDDLE); }

    bool isShiftKeyDown() { return isKeyDown(GLFW_KEY_LEFT_SHIFT) || isKeyDown(GLFW_KEY_RIGHT_SHIFT); }
    bool isAltKeyDown() { return isKeyDown(GLFW_KEY_LEFT_ALT) || isKeyDown(GLFW_KEY_RIGHT_ALT); }
    bool isCtrlKeyDown() { return isKeyDown(GLFW_KEY_LEFT_CONTROL) || isKeyDown(GLFW_KEY_RIGHT_CONTROL); }

    const char* getClipboard() { return glfwGetClipboardString(m_WindowHandle); }
    void setClipboard(const char* str) { glfwSetClipboardString(m_WindowHandle, str); }

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


//    void maximize() {
//        glfwMaximizeWindow(m_WindowHandle);
//    }
//    void restoreMaximize() {
//        glfwRestoreWindow(m_WindowHandle);
//    }


    void PollEvents()
    {
        // Reset FrameStates
        m_MouseDX = 0;
        m_MouseDY = 0;
        m_ScrollDX = 0;
        m_ScrollDY = 0;
        m_IsFramebufferResized = false;

        // PollEvents
        glfwPollEvents();
    }

//    void SwapBuffers()
//    {
//        glfwSwapBuffers(m_WindowHandle);
//    }
//
//    void setVSync(bool vsync) {
//        glfwSwapInterval(vsync ? 1 : 0);
//    }

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

    bool m_MouseGrabbed = false;

};

#endif //ETHERTIA_WINDOW_H
