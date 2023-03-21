//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_WINDOW_H
#define ETHERTIA_WINDOW_H

class GLFWwindow;

class Window
{
public:
    Window(int w, int h, const char* title);
    ~Window();

    static void Init();     // glfwInit(); etc.
    static void Destroy();  // glfwTerminate();

    // reset 'delta' states like mouseDX, isFramebufferResized etc.
    // then:: glfwPollEvents()
    void ProcessEvents();

    bool isCloseRequested();
    bool isFramebufferResized();


    bool isKeyDown(int key);
    bool isMouseDown(int mb);

    bool isCtrlKeyDown();
    bool isShiftKeyDown();
    bool isAltKeyDown();

    bool isMouseLeftDown();
    bool isMouseMiddleDown();
    bool isMouseRightDown();


    GLFWwindow* m_WindowHandle = nullptr;

    bool m_FramebufferResized = false;
};


#endif //ETHERTIA_WINDOW_H
