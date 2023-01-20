//
// Created by Dreamtowards on 2023/1/20.
//



#include <ethertia/util/Log.h>

#include "Vulkan.h"

// 不要手动链接 MoltenVK

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // disable OpenGL
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    GLFWwindow* m_WindowHandle = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);

    Vulkan::init();

    while (!glfwWindowShouldClose(m_WindowHandle))
    {


        glfwPollEvents();
    }

    glfwDestroyWindow(m_WindowHandle);

    glfwTerminate();
}