//
// Created by Dreamtowards on 2023/3/17.
//


#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <map>
#include <set>
#include <fstream>



std::vector<char> loadFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error(Strings::fmt("Failed open file. ", path));
        //Log::warn("Failed open file: ", path);
        //return std::make_pair(nullptr, -1);
    }
    size_t filesize = file.tellg();
    std::vector<char> data(filesize);

    file.seekg(0);
    file.read(data.data(), filesize);
    file.close();

    return data;
}

struct QueueFamilyIndices {
    uint32_t m_GraphicsFamily = -1;
    uint32_t m_PresentFamily = -1;  //Surface Present.

    bool isComplete() {
        return m_GraphicsFamily != -1 && m_PresentFamily != -1;
    }
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_Capabilities;
    std::vector<VkSurfaceFormatKHR> m_Formats;
    std::vector<VkPresentModeKHR> m_PresentModes;

    bool isSwapChainAdequate() {
        return !m_Formats.empty() && !m_PresentModes.empty();
    }
};

class VulkanIntl
{
public:
    inline static bool g_EnableValidationLayer = true;
    inline static std::vector<const char*> g_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    inline static VkDebugUtilsMessengerEXT g_Ext_DebugMessenger = nullptr;

    inline static VkInstance g_vkInstance = nullptr;
    inline static VkPhysicalDevice g_PhysDevice = nullptr;
    inline static VkDevice g_Device = nullptr;  // Logical Device
    inline static VkQueue g_GraphicsQueue = nullptr;
    inline static VkQueue g_PresentQueue = nullptr;  // Surface Present

    inline static VkSurfaceKHR g_SurfaceKHR = nullptr;
    inline static VkSwapchainKHR g_SwapchainKHR = nullptr;
    inline static std::vector<VkImage> g_SwapchainImages;  // auto clean by vk swapchain
    inline static std::vector<VkImageView> g_SwapchainImageViews;
    inline static VkExtent2D g_SwapchainExtent = {};
    inline static VkFormat g_SwapchainImageFormat = {};

    inline static VkRenderPass g_RenderPass = nullptr;
    inline static VkPipelineLayout g_PipelineLayout = nullptr;
    inline static VkPipeline g_GraphicsPipeline = nullptr;
    inline static std::vector<VkFramebuffer> g_SwapchainFramebuffers;

    inline static VkCommandPool g_CommandPool = nullptr;
    inline static VkCommandBuffer g_CommandBuffer = nullptr;

    inline static VkSemaphore g_SemaphoreImageAvailable;
    inline static VkSemaphore g_SemaphoreRenderFinished;
    inline static VkFence g_InflightFence;

};

