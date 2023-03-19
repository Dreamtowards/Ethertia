//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_WINDOW_H
#define ETHERTIA_WINDOW_H

class Window
{
public:
    Window(int w, int h, const char* title);
    ~Window();

    bool isCloseRequested();


    GLFWwindow* m_WindowHandle = nullptr;
};


#endif //ETHERTIA_WINDOW_H
