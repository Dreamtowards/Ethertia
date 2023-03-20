//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_VULKANINTL_H
#define ETHERTIA_VULKANINTL_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <stdexcept>


static void _vk_check_result(VkResult rs)
{
    if (rs != VK_SUCCESS) throw std::runtime_error("[vulkan] Error: VkResult = ");
}
#define VK_CHECK(rs) _vk_check_result(rs)


class VulkanIntl
{
public:
    static void Init(GLFWwindow* glfwWindow);

    static void Destroy();

    static void DrawFrame();

    static void RequestRecreateSwapchain();

    static void SubmitOnetimeCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record);

    struct State
    {
        VkInstance          g_Instance;
        VkPhysicalDevice    g_PhysDevice;
        VkDevice            g_Device;
        VkQueue             g_GraphicsQueue;
        VkQueue             g_PresentQueue;

        VkRenderPass        g_RenderPass;
    };
    static State& GetState();

};

#endif //ETHERTIA_VULKANINTL_H
