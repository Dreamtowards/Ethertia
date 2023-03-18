//
// Created by Dreamtowards on 2023/3/16.
//

#include "Vulkan.h"

#include <set>
#include <chrono>
#include <array>

#include <ethertia/util/Log.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//#define LOADER_NO_OPENGL
//#include <ethertia/util/Loader.h>


inline static bool g_EnableValidationLayer = true;
inline static std::vector<const char*> g_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
};
inline static VkDebugUtilsMessengerEXT g_DebugMessengerEXT = nullptr;

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
inline static VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
inline static VkPipelineLayout g_PipelineLayout = nullptr;
inline static VkPipeline g_GraphicsPipeline = nullptr;
inline static std::vector<VkFramebuffer> g_SwapchainFramebuffers;  // for each Swapchain Image.

inline static VkCommandPool g_CommandPool = nullptr;
inline static std::vector<VkCommandBuffer> g_CommandBuffers;

inline static std::vector<VkSemaphore> g_SemaphoreImageAvailable;  // for each InflightFrame
inline static std::vector<VkSemaphore> g_SemaphoreRenderFinished;
inline static std::vector<VkFence>     g_InflightFence;

inline static int MAX_FRAMES_INFLIGHT = 2;
inline static int g_CurrentFrameInflight = 0;

inline static std::vector<VkBuffer> g_UniformBuffers;  // for each InflightFrame
inline static std::vector<VkDeviceMemory> g_UniformBuffersMemory;
inline static std::vector<void*> g_UniformBuffersMapped;  // 'Persistent Mapping' since vkMapMemory cost.

inline static VkDescriptorPool g_DescriptorPool = nullptr;
inline static std::vector<VkDescriptorSet> g_DescriptorSets;  // for each InflightFrame


// Requires RecreateSwapchain. when Window/Framebuffer resized.
inline static bool g_IsFramebufferResized = false;
inline static GLFWwindow* g_WindowHandle = nullptr;


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



#include <fstream>

std::vector<char> _LoadFile(const std::string& path)
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





struct Vertex
{
    glm::vec2 pos;
    glm::vec3 col;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }


    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, col);
        return attributeDescriptions;
    }
};

const std::vector<Vertex> g_Vertices = {
        {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};
inline static VkBuffer g_VertexBuffer = nullptr;
VkDeviceMemory g_VertexBufferMemory;


#define DECL_UNIFORM_FIELD alignas(16)

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};





class VulkanIntl {
public:

    static void Init(GLFWwindow* glfwWindow)
    {
        g_vkInstance = CreateVkInstance();
        CreateSurface(glfwWindow);
        g_WindowHandle = glfwWindow;

        PickPhysicalDevice();
        CreateLogicalDevice();

        CreateSwapchainAndImageViews();

        CreateRenderPass();
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();
        CreateFramebuffers();

        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects_Semaphores_Fences();


        CreateVertexBuffer();
        CreateUniformBuffers();
        CreateDescriptorPool();
        CreateDescriptorSets();
    }

    static void Destroy()
    {
        vkDeviceWaitIdle(g_Device);

        DestroySwapchain();

        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(g_Device, g_DescriptorSetLayout, nullptr);
        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkDestroyBuffer(g_Device, g_UniformBuffers[i], nullptr);
            vkFreeMemory(g_Device, g_UniformBuffersMemory[i], nullptr);
        }

        vkDestroyBuffer(g_Device, g_VertexBuffer, nullptr);
        vkFreeMemory(g_Device, g_VertexBufferMemory, nullptr);

        vkDestroyPipeline(g_Device, g_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(g_Device, g_PipelineLayout, nullptr);
        vkDestroyRenderPass(g_Device, g_RenderPass, nullptr);

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkDestroySemaphore(g_Device, g_SemaphoreImageAvailable[i], nullptr);
            vkDestroySemaphore(g_Device, g_SemaphoreRenderFinished[i], nullptr);
            vkDestroyFence(g_Device, g_InflightFence[i], nullptr);
        }
        vkDestroyCommandPool(g_Device, g_CommandPool, nullptr);

        vkDestroyDevice(g_Device, nullptr);

        if (g_EnableValidationLayer) {
            extDestroyDebugMessenger(g_vkInstance, g_DebugMessengerEXT, nullptr);
        }
        vkDestroySurfaceKHR(g_vkInstance, g_SurfaceKHR, nullptr);
        vkDestroyInstance(g_vkInstance, nullptr);
    }


    static VkInstance CreateVkInstance()
    {
        VkInstanceCreateInfo vkCreateInfo{};
        vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // Optional Info.
        VkApplicationInfo vkAppInfo{};
        vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.pApplicationName = "TestApp";
        vkAppInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 1);
        vkAppInfo.pEngineName = "No Engine";
        vkAppInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        vkAppInfo.apiVersion = VK_API_VERSION_1_0;
        vkCreateInfo.pApplicationInfo = &vkAppInfo;


        // VkInstance Extensions.
        std::vector<const char*> extensionsRequired;
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (int i = 0; i < glfwExtensionCount; ++i) {
            extensionsRequired.push_back(glfwExtensions[i]);
        }

    #ifdef __APPLE__
        // for prevents VK_ERROR_INCOMPATIBLE_DRIVER err on MacOS MoltenVK sdk.
        extensionsRequired.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        vkCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        // for supports VK_KHR_portability_subset logical-device-extension on MacOS.
        extensionsRequired.push_back("VK_KHR_get_physical_device_properties2");
    #endif

        if (g_EnableValidationLayer) {
            extensionsRequired.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        vkCreateInfo.enabledExtensionCount = extensionsRequired.size();
        vkCreateInfo.ppEnabledExtensionNames = extensionsRequired.data();


    //    {   // Verbose
    //        uint32_t vkExtensionCount = 0;
    //        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);
    //
    //        VkExtensionProperties vkExtProps[vkExtensionCount];
    //        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtProps);
    //
    //        Log::info("{} VkInstance Extension supported: \1", vkExtensionCount);
    //        for (int i = 0; i < vkExtensionCount; ++i) {
    //            std::cout << vkExtProps[i].extensionName << ", ";
    //        }
    //        std::cout << "\n";

    //        Log::info("VkInstance Extensions: \1");
    //        for (auto & i : extensionsRequired) {
    //            std::cout << i << ", ";
    //        }
    //        std::cout << "\n";
    //    }


        // VkInstance Layers
        VkDebugUtilsMessengerCreateInfoEXT debugMessagerInfo;
        if (g_EnableValidationLayer) {
            CheckValidationLayerSupport(g_ValidationLayers);

            vkCreateInfo.enabledLayerCount = g_ValidationLayers.size();
            vkCreateInfo.ppEnabledLayerNames = g_ValidationLayers.data();

            debugMessagerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessagerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugMessagerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugMessagerInfo.pfnUserCallback = debugMessengerCallback;
            debugMessagerInfo.pUserData = nullptr; // Optional
            vkCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessagerInfo;
        } else {
            vkCreateInfo.enabledLayerCount = 0;
            vkCreateInfo.pNext = nullptr;
        }


        VkInstance instance;
        VkResult vkErr = vkCreateInstance(&vkCreateInfo, nullptr, &instance);
        if (vkErr) {
            throw std::runtime_error(Strings::fmt("failed to create vulkan instance. ", vkErr));
        }

        if (g_EnableValidationLayer) {
            // Setup EXT DebugMessenger
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                VkResult err = func(instance, &debugMessagerInfo, nullptr, &g_DebugMessengerEXT);
                assert(err == VK_SUCCESS);
            } else {
                throw std::runtime_error("ext DebugMessenger not present.");
            }
        }
        return instance;
    }

    static void CheckValidationLayerSupport(const std::vector<const char*>& validationLayers) {
        uint32_t layerCount = 0;
        if (vkEnumerateInstanceLayerProperties(&layerCount, nullptr) != VK_SUCCESS)
            throw std::runtime_error("ERR");

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool found = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw std::runtime_error(Strings::fmt("required validation layer not available: {} of available {}", layerName, layerCount));
            }
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

        const char* MSG_SERV = "VERBO";
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) MSG_SERV = "INFO";
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) MSG_SERV = "WARN";
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) MSG_SERV = "ERROR";

        const char* MSG_TYPE = "GENERAL";
        if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) MSG_TYPE = "VALIDATION";
        else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) MSG_TYPE = "PERFORMANCE";

    //    if (messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        std::cerr << Strings::fmt("VkLayer[{}][{}]: ", MSG_SERV, MSG_TYPE) << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    static void extDestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        assert(func);
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }







    static void CreateSurface(GLFWwindow* glfwWindow)
    {
        if (glfwCreateWindowSurface(g_vkInstance, glfwWindow, nullptr, &g_SurfaceKHR) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface via glfw.");
        }
    }








    static void PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(g_vkInstance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw std::runtime_error("failed to find GPU with vulkan support.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(g_vkInstance, &deviceCount, devices.data());

        std::multimap<int, VkPhysicalDevice> candidates;
        for (const auto& device : devices) {
            int score = ratePhysicalDeviceSuitable(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.cbegin()->first > 0) {
            g_PhysDevice = candidates.cbegin()->second;
        } else {
            throw std::runtime_error("failed to find a suitable GPU.");
        }
    }

    static int ratePhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        QueueFamilyIndices queueFamily = findQueueFamilies(physicalDevice);
        if (!queueFamily.isComplete())
            return 0;

        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, g_SurfaceKHR);
        if (!swapchainSupport.isSwapChainAdequate())
            return 0;

        int score = 0;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 800;
        }
        score += deviceProperties.limits.maxImageDimension2D;

        Log::info("Device ", deviceProperties.deviceName);
        return score;
    }










    static void CreateLogicalDevice()
    {
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Queue Family
        QueueFamilyIndices queueFamily = findQueueFamilies(g_PhysDevice);
        float queuePriority = 1.0f;  // 0.0-1.0

        std::set<uint32_t> uniqQueueFamilyIdx = {queueFamily.m_GraphicsFamily, queueFamily.m_PresentFamily};
        std::vector<VkDeviceQueueCreateInfo> arrQueueCreateInfo;
        for (uint32_t queueFamilyIdx : uniqQueueFamilyIdx) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfo.queueFamilyIndex = queueFamilyIdx;
            queueCreateInfo.queueCount = 1;
            arrQueueCreateInfo.push_back(queueCreateInfo);
        }

        createInfo.pQueueCreateInfos = arrQueueCreateInfo.data();
        createInfo.queueCreateInfoCount = arrQueueCreateInfo.size();

        // Device Features
        VkPhysicalDeviceFeatures deviceFeatures{};
        //vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        createInfo.pEnabledFeatures = &deviceFeatures;

        // Device Extensions  (needs check is supported?)
        std::vector<const char*> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef __APPLE__
                "VK_KHR_portability_subset"
#endif
        };
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.enabledExtensionCount = deviceExtensions.size();

        // Device Validation Layer: already deprecated. ValidationLayer only belongs to VkInstance.
        if (g_EnableValidationLayer) {
            createInfo.ppEnabledLayerNames = g_ValidationLayers.data();
            createInfo.enabledLayerCount = g_ValidationLayers.size();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        // VK_KHR_swapchain
        if (vkCreateDevice(g_PhysDevice, &createInfo, nullptr, &g_Device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device.");
        }

        // Get Queue by the way.
        vkGetDeviceQueue(g_Device, queueFamily.m_GraphicsFamily, 0, &g_GraphicsQueue);
        vkGetDeviceQueue(g_Device, queueFamily.m_PresentFamily, 0, &g_PresentQueue);
    }

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice)
    {
        QueueFamilyIndices queueFamilyIdxs;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        assert(queueFamilyCount > 0);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        // todo: Find the BestFit queue for specific QueueFlag. https://stackoverflow.com/a/57210037
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueFamilyIdxs.m_GraphicsFamily = i;
            }

            VkBool32 supportPresent = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, g_SurfaceKHR, &supportPresent);
            if (supportPresent)
                queueFamilyIdxs.m_PresentFamily = i;

            if (queueFamilyIdxs.isComplete())
                break;

//        Log::info("QueueFamily[{}]: {} :: Graphics: {}, Compute: {}, Transfer: {}",
//                  i, std::bitset<sizeof(VkQueueFlags)>(queueFamily.queueFlags),
//                  queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT,
//                  queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT,
//                  queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT);

            i++;
        }

        return queueFamilyIdxs;
    }












    static void CreateSwapchainAndImageViews() {
        SwapchainSupportDetails swapchainDetails = querySwapchainSupport(g_PhysDevice, g_SurfaceKHR);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainDetails.m_Formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainDetails.m_PresentModes);
        VkExtent2D extent = chooseSwapExtent(swapchainDetails.m_Capabilities);
        g_SwapchainExtent = extent;

        // tri buf
        uint32_t imageCount = swapchainDetails.m_Capabilities.minImageCount + 1;
        if (swapchainDetails.m_Capabilities.maxImageCount > 0 && imageCount > swapchainDetails.m_Capabilities.maxImageCount) {
            imageCount = swapchainDetails.m_Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = g_SurfaceKHR;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;  // normally 1, except VR.
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        g_SwapchainImageFormat = surfaceFormat.format;

        // Image Share Mode. Queue Family.
        QueueFamilyIndices queueFamily = findQueueFamilies(g_PhysDevice);
        uint32_t queueFamilyIdxs[] = {queueFamily.m_GraphicsFamily, queueFamily.m_PresentFamily};

        if (queueFamily.m_GraphicsFamily != queueFamily.m_PresentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIdxs;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;  // most performance.
            createInfo.queueFamilyIndexCount = 0;  // opt
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapchainDetails.m_Capabilities.currentTransform;  // Non transform
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // Non Alpha.
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;  // true: not care the pixels behind other windows for vk optims.  but may cause problem when we read the pixels.
        createInfo.oldSwapchain = nullptr;

        VkResult err = vkCreateSwapchainKHR(g_Device, &createInfo, nullptr, &g_SwapchainKHR);
        if (err != VK_SUCCESS)
            throw std::runtime_error("failed to create vk swapchain khr.");

        // Get Swapchain Images.
        int expectedImageCount = imageCount;
        vkGetSwapchainImagesKHR(g_Device, g_SwapchainKHR, &imageCount, nullptr);
        assert(expectedImageCount == imageCount);

        g_SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(g_Device, g_SwapchainKHR, &imageCount, g_SwapchainImages.data());



        // Image Views
        g_SwapchainImageViews.resize(imageCount);
        for (int i = 0; i < imageCount; ++i) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = g_SwapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;      // except VR.

            if (vkCreateImageView(g_Device, &createInfo, nullptr, &g_SwapchainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create sawpchain imageviews.");
            }
        }
    }

    static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        SwapchainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.m_Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if (formatCount) {
            details.m_Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.m_Formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
        if (presentModeCount) {
            details.m_PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.m_PresentModes.data());
        }

        return details;
    }

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return {VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }
        return availableFormats[0];
    }
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        if (Collections::find(availablePresentModes, VK_PRESENT_MODE_MAILBOX_KHR) != -1)
            return VK_PRESENT_MODE_MAILBOX_KHR;
        if (Collections::find(availablePresentModes, VK_PRESENT_MODE_IMMEDIATE_KHR) != -1)
            return VK_PRESENT_MODE_IMMEDIATE_KHR;

        return VK_PRESENT_MODE_FIFO_KHR;  // FIFO_KHR is vk guaranteed available.
    }
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;

        int width, height;
        glfwGetFramebufferSize(g_WindowHandle, &width, &height);
        VkExtent2D extent = { (uint32_t)width, (uint32_t)height };
        extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return extent;
    }



    static void DestroySwapchain()
    {
        for (auto fb : g_SwapchainFramebuffers) {
            vkDestroyFramebuffer(g_Device, fb, nullptr);
        }
        for (auto imageview : g_SwapchainImageViews) {
            vkDestroyImageView(g_Device, imageview, nullptr);
        }
        vkDestroySwapchainKHR(g_Device, g_SwapchainKHR, nullptr);
    }

    static void RecreateSwapchain()
    {
        vkDeviceWaitIdle(g_Device);
        DestroySwapchain();
        Log::info("RecreateSwapchain");

        CreateSwapchainAndImageViews();
        CreateFramebuffers();
    }




















    static void CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = g_SwapchainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(g_Device, &renderPassInfo, nullptr, &g_RenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass.");
        }
    }




















    static VkShaderModule CreateShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(g_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    static void CreateGraphicsPipeline() {
        auto vertShaderCode = _LoadFile("/Users/dreamtowards/Documents/YouRepository/Ethertia/src/ethertia/render/vulkan/vert.spv");
        auto fragShaderCode = _LoadFile("/Users/dreamtowards/Documents/YouRepository/Ethertia/src/ethertia/render/vulkan/frag.spv");

        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;  // tmp edit
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &g_DescriptorSetLayout;

        if (vkCreatePipelineLayout(g_Device, &pipelineLayoutInfo, nullptr, &g_PipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = g_PipelineLayout;
        pipelineInfo.renderPass = g_RenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_GraphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline.");
        }

        vkDestroyShaderModule(g_Device, fragShaderModule, nullptr);
        vkDestroyShaderModule(g_Device, vertShaderModule, nullptr);
    }


























    static void CreateFramebuffers()
    {
        g_SwapchainFramebuffers.resize(g_SwapchainImageViews.size());

        for (size_t i = 0; i < g_SwapchainImageViews.size(); i++) {
            VkImageView attachments[] = { g_SwapchainImageViews[i] };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = g_RenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = g_SwapchainExtent.width;
            framebufferInfo.height = g_SwapchainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(g_Device, &framebufferInfo, nullptr, &g_SwapchainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }


    static void CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(g_PhysDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.m_GraphicsFamily;

        if (vkCreateCommandPool(g_Device, &poolInfo, nullptr, &g_CommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool.");
        }
    }


    static void CreateCommandBuffer()
    {
        g_CommandBuffers.resize(MAX_FRAMES_INFLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = g_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = g_CommandBuffers.size();

        if (vkAllocateCommandBuffers(g_Device, &allocInfo, g_CommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }


    static void CreateSyncObjects_Semaphores_Fences()
    {
        g_SemaphoreImageAvailable.resize(MAX_FRAMES_INFLIGHT);
        g_SemaphoreRenderFinished.resize(MAX_FRAMES_INFLIGHT);
        g_InflightFence.resize(MAX_FRAMES_INFLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            if (vkCreateSemaphore(g_Device, &semaphoreInfo, nullptr, &g_SemaphoreImageAvailable[i]) != VK_SUCCESS ||
                vkCreateSemaphore(g_Device, &semaphoreInfo, nullptr, &g_SemaphoreRenderFinished[i]) != VK_SUCCESS ||
                vkCreateFence(g_Device, &fenceInfo, nullptr, &g_InflightFence[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create semaphores or fence.");
            }
        }
    }




    static void CreateBuffer(VkDeviceSize size,
                             VkBuffer& buffer,
                             VkDeviceMemory& bufferMemory,
                             VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VkMemoryPropertyFlags properties =
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(g_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create a vertex buffer");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(g_Device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(g_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory.");
        }

        vkBindBufferMemory(g_Device, buffer, bufferMemory, 0);

    }

    static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = g_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer cmdbuf;
        vkAllocateCommandBuffers(g_Device, &allocInfo, &cmdbuf);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(cmdbuf, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        vkCmdCopyBuffer(cmdbuf, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(cmdbuf);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdbuf;

        vkQueueSubmit(g_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(g_GraphicsQueue);

        vkFreeCommandBuffers(g_Device, g_CommandPool, 1, &cmdbuf);
    }

    static void CreateVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(g_Vertices[0]) * g_Vertices.size();



        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(bufferSize, stagingBuffer, stagingBufferMemory,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        vkMapMemory(g_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, g_Vertices.data(), bufferSize);
        vkUnmapMemory(g_Device, stagingBufferMemory);

        CreateBuffer(bufferSize, g_VertexBuffer, g_VertexBufferMemory,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        CopyBuffer(stagingBuffer, g_VertexBuffer, bufferSize);

        vkDestroyBuffer(g_Device, stagingBuffer, nullptr);
        vkFreeMemory(g_Device, stagingBufferMemory, nullptr);
    }

    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(g_PhysDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }









    static void CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(g_Device, &layoutInfo, nullptr, &g_DescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout.");
        }
    }

    static void CreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        g_UniformBuffers.resize(MAX_FRAMES_INFLIGHT);
        g_UniformBuffersMemory.resize(MAX_FRAMES_INFLIGHT);
        g_UniformBuffersMapped.resize(MAX_FRAMES_INFLIGHT);

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            CreateBuffer(bufferSize,
                         g_UniformBuffers[i],
                         g_UniformBuffersMemory[i],
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkMapMemory(g_Device, g_UniformBuffersMemory[i], 0, sizeof(UniformBufferObject), 0,
                        &g_UniformBuffersMapped[i]);
        }
    }

    static void UpdateUniformBuffer(uint32_t currframe)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), g_SwapchainExtent.width / (float) g_SwapchainExtent.height, 0.1f, 10.0f);

        ubo.proj[1][1] *= -1;

        memcpy(g_UniformBuffersMapped[currframe], &ubo, sizeof(ubo));
    }


    static void CreateDescriptorPool()
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = MAX_FRAMES_INFLIGHT;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = MAX_FRAMES_INFLIGHT;

        if (vkCreateDescriptorPool(g_Device, &poolInfo, nullptr, &g_DescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool.");
        }
    }

    static void CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_INFLIGHT, g_DescriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = g_DescriptorPool;
        allocInfo.descriptorSetCount = MAX_FRAMES_INFLIGHT;
        allocInfo.pSetLayouts = layouts.data();

        g_DescriptorSets.resize(MAX_FRAMES_INFLIGHT);
        if (vkAllocateDescriptorSets(g_Device, &allocInfo, g_DescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets.");
        }

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = g_UniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = g_DescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(g_Device, 1, &descriptorWrite, 0, nullptr);

        }
    }
};




























static void RecordCommandBuffer(VkCommandBuffer cmdbuf, uint32_t imageIdx)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(cmdbuf, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = g_RenderPass;
    renderPassInfo.framebuffer = g_SwapchainFramebuffers[imageIdx];

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = g_SwapchainExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmdbuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, g_GraphicsPipeline);


    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = g_SwapchainExtent.width;
    viewport.height = g_SwapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmdbuf, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = g_SwapchainExtent;
    vkCmdSetScissor(cmdbuf, 0, 1, &scissor);


    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdbuf, 0, 1, &g_VertexBuffer, offsets);

    vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, g_PipelineLayout, 0, 1,
                            &g_DescriptorSets[g_CurrentFrameInflight], 0, nullptr);  // i?
    vkCmdDraw(cmdbuf, g_Vertices.size(), 1, 0, 0);

    vkCmdEndRenderPass(cmdbuf);

    if (vkEndCommandBuffer(cmdbuf) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer.");
    }
}



static void DrawFrame()
{
    const int currframe = g_CurrentFrameInflight;

    vkWaitForFences(g_Device, 1, &g_InflightFence[currframe], VK_TRUE, UINT64_MAX);

    uint32_t imageIdx;
    VkResult rs = vkAcquireNextImageKHR(g_Device, g_SwapchainKHR, UINT64_MAX, g_SemaphoreImageAvailable[currframe],
                                        VK_NULL_HANDLE, &imageIdx);
    if (rs == VK_ERROR_OUT_OF_DATE_KHR) {
        VulkanIntl::RecreateSwapchain();
        return;
    } else if (!(rs == VK_SUCCESS || rs == VK_SUBOPTIMAL_KHR)) {
        throw std::runtime_error("failed to acquire swapchain image.");
    }

    vkResetFences(g_Device, 1, &g_InflightFence[currframe]);

    vkResetCommandBuffer(g_CommandBuffers[currframe], 0);
    RecordCommandBuffer(g_CommandBuffers[currframe], imageIdx);

    VulkanIntl::UpdateUniformBuffer(currframe);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &g_CommandBuffers[currframe];

    VkSemaphore waitSemaphores[] = { g_SemaphoreImageAvailable[currframe] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    VkSemaphore signalSemaphores[] = { g_SemaphoreRenderFinished[currframe] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(g_GraphicsQueue, 1, &submitInfo, g_InflightFence[currframe]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer.");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &g_SwapchainKHR;
    presentInfo.pImageIndices = &imageIdx;

    rs = vkQueuePresentKHR(g_PresentQueue, &presentInfo);
    if (rs == VK_ERROR_OUT_OF_DATE_KHR || rs == VK_SUBOPTIMAL_KHR || g_IsFramebufferResized) {
        g_IsFramebufferResized = false;
        VulkanIntl::RecreateSwapchain();
    } else if (rs != VK_SUCCESS) {
        throw std::runtime_error("failed to present swapchain image.");
    }

    // vkQueueWaitIdle(g_vkPresentQueue);  // BigWaste on GPU.

    g_CurrentFrameInflight = (g_CurrentFrameInflight + 1) % MAX_FRAMES_INFLIGHT;
}




#include <ethertia/util/BenchmarkTimer.h>

static void glfw_framebuffer_resized(GLFWwindow* glfwWindow, int w, int h) {
    g_IsFramebufferResized = true;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // disable OpenGL

    GLFWwindow* glfwWindow = glfwCreateWindow(80, 60, "Test", nullptr, nullptr);

    glfwSetFramebufferSizeCallback(glfwWindow, glfw_framebuffer_resized);

    VulkanIntl::Init(glfwWindow);


    int n = 0;
    float t = 0;
    while (!glfwWindowShouldClose(glfwWindow))
    {
        if (t > 1.0f) {
            Log::info("{} frames in {} sec", n, t);
            t = 0; n = 0;
        }
        ++n;
        BENCHMARK_TIMER_VAL(&t);
        glfwPollEvents();

        DrawFrame();
    }

    VulkanIntl::Destroy();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}