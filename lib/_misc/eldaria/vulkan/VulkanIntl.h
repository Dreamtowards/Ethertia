//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_VULKANINTL_H
#define ETHERTIA_VULKANINTL_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <stdexcept>

#include "vkx.h"


class VulkanIntl
{
public:
    static void Init(GLFWwindow* glfwWindow);

    static void Destroy();

    static void DrawFrame();

    static void RequestRecreateSwapchain();



    static VkImageView getTestImgView();
};


#endif //ETHERTIA_VULKANINTL_H
