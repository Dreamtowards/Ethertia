//
// Created by Dreamtowards on 2023/1/20.
//

#ifndef ETHERTIA_VULKAN_H
#define ETHERTIA_VULKAN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <map>

#include <ethertia/util/Collections.h>


class Vulkan
{
public:

    static void Init(GLFWwindow* glfwWindow);

    static void Destroy();

};

#endif //ETHERTIA_VULKAN_H
