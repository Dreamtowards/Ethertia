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

    // reset 'delta' states like mouseDX, isFramebufferResized etc.
    // then:: glfwPollEvents()
    void ProcessEvents();

    bool isCloseRequested();

    bool isFramebufferResized();


    GLFWwindow* m_WindowHandle = nullptr;

    bool m_FramebufferResized = false;
};


#endif //ETHERTIA_WINDOW_H
