//
// Created by Dreamtowards on 2023/3/16.
//

#include "Vulkan.h"

#include <ethertia/util/Log.h>

#include <set>

inline static bool g_EnableValidationLayer = true;
inline static std::vector<const char*> g_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
};
inline static VkDebugUtilsMessengerEXT g_VkExt_DebugMessenger = nullptr;

inline static VkInstance g_vkInstance = nullptr;

inline static VkSurfaceKHR g_vkSurfaceKHR = nullptr;

inline static VkPhysicalDevice g_vkPhysDevice = nullptr;
inline static VkDevice g_vkDevice = nullptr;  // Logical Device

inline static VkQueue g_vkGraphicsQueue = nullptr;
inline static VkQueue g_vkPresentQueue = nullptr;  // Surface Present

inline static VkSwapchainKHR g_vkSwapchainKHR = nullptr;
inline static std::vector<VkImage> g_SwapchainImages;  // auto clean by vk swapchain
inline static std::vector<VkImageView> g_SwapchainImageViews;





class GraphicsPipeline
{
public:

    static VkShaderModule newShaderModule(VkDevice device, size_t codeSize, uint32_t* codeData) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = codeSize;
        createInfo.pCode = codeData;

        VkShaderModule shaderModule;
        VkResult vkErr = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
        if (vkErr != VK_SUCCESS)
            throw std::runtime_error("failed to create shader module.");

        return shaderModule;
    }

    static void newGraphicsPipeline(VkDevice* device) {

//            VkShaderModule vshModule = newShaderModule(device, );
//
//            VkPipelineShaderStageCreateInfo vshStageInfo{};
//            vshStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//            vshStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//            vshStageInfo.module = vshModule;
//            vshStageInfo.pName = "main";
        // vshStageInfo.pSpecializationInfo

//            vkDestroyShaderModule(device, vshModule, nullptr);
//            vkDestroyShaderModule(device, fshModule, nullptr);
    }


};












////////////// Logical Device & Queue //////////////

struct QueueFamilyIndices {
    uint32_t m_GraphicsFamily = -1;
    uint32_t m_PresentFamily = -1;  //Surface Present.

    bool isComplete() {
        return m_GraphicsFamily != -1 && m_PresentFamily != -1;
    }
};

static QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    QueueFamilyIndices queueFamilyIdxs;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    if (queueFamilyCount == 0) {
        Log::warn("No QueueFamilies.");
        return queueFamilyIdxs;
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());


    // todo: Find the BestFit queue for specific QueueFlag. https://stackoverflow.com/a/57210037

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIdxs.m_GraphicsFamily = i;
        }

        VkBool32 supportPresent = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresent);
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



static VkDevice InitLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue* _Dst_GraphicsQueue, VkQueue* _Dst_PresentQueue)
{
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // Queue Family
    QueueFamilyIndices queueFamily = _FindQueueFamilies(physicalDevice, surface);
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

    VkDevice device;
    VkResult err = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
    if (err != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device.");
    }

    // Get Queue by the way.
    vkGetDeviceQueue(device, queueFamily.m_GraphicsFamily, 0, _Dst_GraphicsQueue);
    vkGetDeviceQueue(device, queueFamily.m_PresentFamily, 0, _Dst_PresentQueue);

    return device;
}














//////////////// SwapChain ////////////////

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_Capabilities;
    std::vector<VkSurfaceFormatKHR> m_Formats;
    std::vector<VkPresentModeKHR> m_PresentModes;

    bool isSwapChainAdequate() {
        return !m_Formats.empty() && !m_PresentModes.empty();
    }
};
static SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

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

static VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
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
static VkPresentModeKHR _ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    if (Collections::find(availablePresentModes, VK_PRESENT_MODE_MAILBOX_KHR) != -1)
        return VK_PRESENT_MODE_MAILBOX_KHR;
    if (Collections::find(availablePresentModes, VK_PRESENT_MODE_IMMEDIATE_KHR) != -1)
        return VK_PRESENT_MODE_IMMEDIATE_KHR;

    return VK_PRESENT_MODE_FIFO_KHR;  // FIFO_KHR is vk guaranteed available.
}
static VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    VkExtent2D extent = {1280, 720};
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}

static VkSwapchainKHR InitSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
                                    std::vector<VkImage>& _Dst_SwapchainImages, std::vector<VkImageView>& _Dst_SwapchainImageViews) {
    SwapChainSupportDetails swapChainDetails = _QuerySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = _ChooseSwapSurfaceFormat(swapChainDetails.m_Formats);
    VkPresentModeKHR presentMode = _ChooseSwapPresentMode(swapChainDetails.m_PresentModes);
    VkExtent2D extent = _ChooseSwapExtent(swapChainDetails.m_Capabilities);

    // tri buf
    uint32_t imageCount = swapChainDetails.m_Capabilities.minImageCount + 1;
    if (swapChainDetails.m_Capabilities.maxImageCount > 0 && imageCount > swapChainDetails.m_Capabilities.maxImageCount) {
        imageCount = swapChainDetails.m_Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;  // normally 1, except VR.
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Image Share Mode. Queue Family.
    QueueFamilyIndices queueFamily = _FindQueueFamilies(physicalDevice, surface);
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

    createInfo.preTransform = swapChainDetails.m_Capabilities.currentTransform;  // Non transform
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // Non Alpha.
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;  // true: not care the pixels behind other windows for vk optims.  but may cause problem when we read the pixels.
    createInfo.oldSwapchain = nullptr;

    VkSwapchainKHR swapchain;
    VkResult err = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
    if (err != VK_SUCCESS)
        throw std::runtime_error("failed to create vk swapchain khr.");

    // Get Swapchain Images.
    int expectedImageCount = imageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    assert(expectedImageCount == imageCount);

    _Dst_SwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, _Dst_SwapchainImages.data());

    // Image Views
    _Dst_SwapchainImageViews.resize(imageCount);
    for (int i = 0; i < imageCount; ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _Dst_SwapchainImages[i];
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

        if (vkCreateImageView(device, &createInfo, nullptr, &_Dst_SwapchainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create sawpchain imageviews.");
        }
    }

    return swapchain;
}




////////////// Physical Device //////////////

static int _RatePhysicalDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    QueueFamilyIndices queueFamily = _FindQueueFamilies(physicalDevice, surface);
    if (!queueFamily.isComplete())
        return 0;

    SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(physicalDevice, surface);
    if (!swapChainSupport.isSwapChainAdequate())
        return 0;

    int score = 0;
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 800;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    Log::info("Device ", deviceProperties.deviceName);
    return score;
}



// the arg VkSurfaceKHR is used to rate a device that whether supports Surface Present. 　
static VkPhysicalDevice InitPhysicalDevice(VkSurfaceKHR surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(g_vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw std::runtime_error("failed to find GPU with vulkan support.");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(g_vkInstance, &deviceCount, devices.data());

    VkPhysicalDevice physicalDevice = nullptr;

    std::multimap<int, VkPhysicalDevice> candidates;
    for (const auto& device : devices) {
        int score = _RatePhysicalDeviceSuitable(device, surface);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.cbegin()->first > 0) {
        physicalDevice = candidates.cbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU.");
    }

    return physicalDevice;
}










/////////// Validation Layer & Debug Error Message ///////////

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
            throw std::runtime_error(Strings::fmt("required validation layer not available: {} of available {}: {}", layerName, layerCount));
        }
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL _vkDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    const char* MSG_SERV = "VERBO";
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) MSG_SERV = "INFO";
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) MSG_SERV = "WARN";
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) MSG_SERV = "ERROR";

    const char* MSG_TYPE = "GENERAL";
    if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) MSG_TYPE = "VALIDATION";
    else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) MSG_TYPE = "PERFORMANCE";

    // if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)

    if (messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    std::cerr << Strings::fmt("VkLayer[{}][{}]: ", MSG_SERV, MSG_TYPE) << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
static VkDebugUtilsMessengerCreateInfoEXT _VkExt_DebugMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = _vkDebugCallback;
    createInfo.pUserData = nullptr; // Optional
    return createInfo;
}
static VkResult _VkExt_CreateDebugMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                              const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
static void _VkExt_DestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    assert(func);
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static void initVkDebugMessage(VkInstance instance)
{
    assert(g_EnableValidationLayer);

    VkDebugUtilsMessengerCreateInfoEXT createInfo = _VkExt_DebugMessengerCreateInfo();

    if (_VkExt_CreateDebugMessenger(instance, &createInfo, nullptr, &g_VkExt_DebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}


























static VkInstance InitVkInstance()
{
    {
        uint32_t vkExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);

        VkExtensionProperties vkExtProps[vkExtensionCount];
        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtProps);

        Log::info("{} VkInstance Extension supported: \1", vkExtensionCount);
        for (int i = 0; i < vkExtensionCount; ++i) {
            std::cout << vkExtProps[i].extensionName << ", ";
        }
        std::cout << "\n";
    }

    VkInstanceCreateInfo vkCreateInfo{};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    {
        // Optional Info.
        VkApplicationInfo vkAppInfo{};
        vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.pApplicationName = "TestApp";
        vkAppInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 1);
        vkAppInfo.pEngineName = "No Engine";
        vkAppInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        vkAppInfo.apiVersion = VK_API_VERSION_1_0;
        vkCreateInfo.pApplicationInfo = &vkAppInfo;
    }


    // VkInstance Extensions.
    std::vector<const char*> extensionsRequired;
    {
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
    }
    Log::info("VkInstance Extensions: \1");
    for (int i = 0; i < extensionsRequired.size(); ++i) {
        std::cout << extensionsRequired[i] << ", ";
    }
    std::cout << "\n";


    // VkInstance Layers
    if (g_EnableValidationLayer) {
        CheckValidationLayerSupport(g_ValidationLayers);

        vkCreateInfo.enabledLayerCount = g_ValidationLayers.size();
        vkCreateInfo.ppEnabledLayerNames = g_ValidationLayers.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = _VkExt_DebugMessengerCreateInfo();
        vkCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
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
        initVkDebugMessage(instance);
    }

    return instance;
}


///////////// SurfaceKHR /////////////

static void InitSurface(VkInstance vkInstance, GLFWwindow* glfwWindow, VkSurfaceKHR* _Dst_Surface)
{

    VkResult vkErr = glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, _Dst_Surface);

    if (vkErr != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface via glfw.");
    }

}



// 1. VkInstance (+ValidationLayer)
// 2. Init Surface.
// 3. Pick a PhysicalDevice
// 4. Create a LogicalDevice and QueueFamilies

void Vulkan::Init(GLFWwindow* glfwWindow)
{
    g_vkInstance = InitVkInstance();

    InitSurface(g_vkInstance, glfwWindow, &g_vkSurfaceKHR);

    g_vkPhysDevice = InitPhysicalDevice(g_vkSurfaceKHR);
    g_vkDevice = InitLogicalDevice(g_vkPhysDevice, g_vkSurfaceKHR, &g_vkGraphicsQueue, &g_vkPresentQueue);

    g_vkSwapchainKHR = InitSwapChain(g_vkPhysDevice, g_vkDevice, g_vkSurfaceKHR, g_SwapchainImages, g_SwapchainImageViews);

    // initCommandPool();
}

void Vulkan::Destroy()
{
    if (g_EnableValidationLayer) {
        _VkExt_DestroyDebugMessenger(g_vkInstance, g_VkExt_DebugMessenger, nullptr);
    }
    for (auto imageview : g_SwapchainImageViews) {
        vkDestroyImageView(g_vkDevice, imageview, nullptr);
    }

    vkDestroySwapchainKHR(g_vkDevice, g_vkSwapchainKHR, nullptr);
    vkDestroyDevice(g_vkDevice, nullptr);
    vkDestroySurfaceKHR(g_vkInstance, g_vkSurfaceKHR, nullptr);
    vkDestroyInstance(g_vkInstance, nullptr);
}




