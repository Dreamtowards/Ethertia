//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_VULKANINTL_H
#define ETHERTIA_VULKANINTL_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


class VulkanIntl
{
public:
    static void Init(void* glfwWindow);

    static void Destroy();

    static void DrawFrame();

    static void RequestRecreateSwapchain();


    inline static VkInstance        g_Instance  = nullptr;
    inline static VkPhysicalDevice  g_PhysDevice= nullptr;
    inline static VkDevice          g_Device    = nullptr;  // Logical Device

    inline static VkRenderPass      g_RenderPass = nullptr;

    inline static VkQueue g_GraphicsQueue = nullptr;
    inline static VkQueue g_PresentQueue = nullptr;  // Surface Present

    inline static VkSurfaceKHR          g_SurfaceKHR = nullptr;
    inline static VkSwapchainKHR        g_SwapchainKHR = nullptr;
    inline static std::vector<VkImage>  g_SwapchainImages;  // auto clean by vk swapchain
    inline static std::vector<VkImageView> g_SwapchainImageViews;
    inline static VkExtent2D            g_SwapchainExtent = {};
    inline static VkFormat              g_SwapchainImageFormat = {};
    inline static std::vector<VkFramebuffer> g_SwapchainFramebuffers;  // for each Swapchain Image.

    inline static VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    inline static VkPipelineLayout  g_PipelineLayout = nullptr;
    inline static VkPipeline        g_GraphicsPipeline = nullptr;

    inline static VkCommandPool g_CommandPool = nullptr;
    inline static std::vector<VkCommandBuffer> g_CommandBuffers;

    inline static std::vector<VkSemaphore> g_SemaphoreImageAvailable;  // for each InflightFrame   ImageAcquired, RenderComplete
    inline static std::vector<VkSemaphore> g_SemaphoreRenderFinished;
    inline static std::vector<VkFence>     g_InflightFence;

    inline static int MAX_FRAMES_INFLIGHT = 2;
    inline static int g_CurrentFrameInflight = 0;

    inline static std::vector<VkBuffer> g_UniformBuffers;  // for each InflightFrame
    inline static std::vector<VkDeviceMemory> g_UniformBuffersMemory;
    inline static std::vector<void*> g_UniformBuffersMapped;  // 'Persistent Mapping' since vkMapMemory cost.

    inline static VkDescriptorPool g_DescriptorPool = nullptr;
    inline static std::vector<VkDescriptorSet> g_DescriptorSets;  // for each InflightFrame

    inline static VkImage g_TextureImage = nullptr;
    inline static VkDeviceMemory g_TextureImageMemory = nullptr;
    inline static VkImageView g_TextureImageView = nullptr;
    inline static VkSampler g_TextureSampler = nullptr;

    inline static VkImage g_DepthImage = nullptr;
    inline static VkDeviceMemory g_DepthImageMemory = nullptr;
    inline static VkImageView g_DepthImageView = nullptr;

    inline static bool g_RecreateSwapchainRequested = false;  // when Window/Framebuffer resized.
    inline static GLFWwindow* g_WindowHandle = nullptr;

    inline static bool g_EnableValidationLayer = true;
    inline static std::vector<const char*> g_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    inline static VkDebugUtilsMessengerEXT g_DebugMessengerEXT = nullptr;


    struct QueueFamilyIndices {
        uint32_t m_GraphicsFamily = -1;
        uint32_t m_PresentFamily = -1;  //Surface Present.

        bool isComplete() const {
            return m_GraphicsFamily != -1 && m_PresentFamily != -1;
        }
    };

    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR m_Capabilities{};
        std::vector<VkSurfaceFormatKHR> m_Formats;
        std::vector<VkPresentModeKHR> m_PresentModes;

        bool isSwapChainAdequate() const {
            return !m_Formats.empty() && !m_PresentModes.empty();
        }
    };

};

#endif //ETHERTIA_VULKANINTL_H
