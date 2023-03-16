//
// Created by Dreamtowards on 2023/1/20.
//



#include <ethertia/util/Log.h>

#include <ethertia/render/vulkan/Vulkan.h>

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // disable OpenGL
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    GLFWwindow* glfwWindow = glfwCreateWindow(1280, 720, "Test", nullptr, nullptr);

    Vulkan::Init(glfwWindow);

    while (!glfwWindowShouldClose(glfwWindow))
    {


        glfwPollEvents();
    }

    Vulkan::Destroy();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}