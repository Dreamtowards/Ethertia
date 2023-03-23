//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_VULKANINTL_H
#define ETHERTIA_VULKANINTL_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <stdexcept>


class VulkanIntl
{
public:
    static void Init(GLFWwindow* glfwWindow);

    static void Destroy();

    static void DrawFrame();

    static void RequestRecreateSwapchain();

    // on Graphics Queue., alloc CommandBuffer from main-thread CommandPool.
    static void SubmitOnetimeCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record);

    struct State
    {
        VkInstance          g_Instance;
        VkPhysicalDevice    g_PhysDevice;
        VkDevice            g_Device;
        VkQueue             g_GraphicsQueue;
        VkQueue             g_PresentQueue;

        VkRenderPass        g_RenderPass;

        VkCommandPool       g_CommandPool;

        VkSampler           g_TextureSampler;
    };
    static State& GetState(bool init = false);


    // helper func. since g_Device be needed frequently.
    static VkDevice vkDevice() {
        return GetState().g_Device;
    }

    static VkImageView getTestImgView();
};


static void _vk_check_result(VkResult rs)
{
    if (rs != VK_SUCCESS) throw std::runtime_error("[vulkan] Error: VkResult = ");
}
#define VK_CHECK(rs) _vk_check_result(rs)

#define VK_CHECK_MSG(rs, msg) _vk_check_result(rs)



#endif //ETHERTIA_VULKANINTL_H
