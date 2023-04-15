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


static void _vk_check_result(VkResult rs)
{
    if (rs != VK_SUCCESS)
        throw std::runtime_error("[vulkan] Error: VkResult = ");
}
#define VK_CHECK(rs) _vk_check_result(rs)

#define VK_CHECK_MSG(rs, msg) _vk_check_result(rs)



#endif //ETHERTIA_VULKANINTL_H
