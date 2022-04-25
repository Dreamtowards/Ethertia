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

public:

    int initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // OSX Required.

        window = glfwCreateWindow(600, 420, "Dysplay", nullptr, nullptr);
        if (!window)
            throw std::runtime_error("Failed to init GLFW window.");

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to init GLAD.");

        glfwSetWindowCloseCallback(window, onWindowClose);

        return 0;
    }

    void updateWindow()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    bool isCloseRequested() {
        return glfwWindowShouldClose(window);
    }

    static double getPreciseTime() {
        return glfwGetTime();
    }


    static void onWindowClose(GLFWwindow* window);

};

#endif //ETHERTIA_WINDOW_H
