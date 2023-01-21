//
// Created by Dreamtowards on 2023/1/20.
//

#ifndef ETHERTIA_VULKAN_H
#define ETHERTIA_VULKAN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <map>


class Vulkan
{
public:
    inline static bool _ValidationLayer = true;
    inline static std::vector<const char*> _ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    inline static VkDebugUtilsMessengerEXT _DebugMessenger = nullptr;

    inline static VkInstance vkInstance = nullptr;
    inline static VkPhysicalDevice vkPhysDevice = nullptr;
    inline static VkDevice vkDevice = nullptr;

    inline static VkQueue vkGraphicsQueue = nullptr;
    inline static VkQueue vkPresentQueue = nullptr;  // Surface Present

    inline static VkSurfaceKHR vkSurfaceKHR = nullptr;

    static void init(GLFWwindow* glfwWindow)
    {
        vkInstance = initVkInstance();

        initSurface(vkInstance, glfwWindow, &vkSurfaceKHR);

        vkPhysDevice = initPhysicalDevice(vkSurfaceKHR);
        vkDevice = initLogicalDevice(vkPhysDevice, vkSurfaceKHR, &vkGraphicsQueue, &vkPresentQueue);

        // initCommandPool();
    }

    static void deinit()
    {
        if (_ValidationLayer) {
            _DestroyDebugUtilsMessengerEXT(vkInstance, _DebugMessenger, nullptr);
        }

        vkDestroyDevice(vkDevice, nullptr);
        vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, nullptr);
        vkDestroyInstance(vkInstance, nullptr);
    }


private:

    static VkInstance initVkInstance()
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

            // for prevents VK_ERROR_INCOMPATIBLE_DRIVER err on MacOS MoltenVK sdk.
            extensionsRequired.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            vkCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

            if (_ValidationLayer) {
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
        if (_ValidationLayer) {
            _CheckValidationLayerSupport(_ValidationLayers);

            vkCreateInfo.enabledLayerCount = _ValidationLayers.size();
            vkCreateInfo.ppEnabledLayerNames = _ValidationLayers.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = _DebugMessengerCreateInfo();
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


        if (_ValidationLayer) {
            initVkDebugMessage(instance);
        }

        return instance;
    }









    /////////// Validation Layer & Debug Error Message ///////////

    static void _CheckValidationLayerSupport(const std::vector<const char*>& validationLayers) {
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

        std::cerr << Strings::fmt("VkLayer[{}][{}]: ", MSG_SERV, MSG_TYPE) << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    static VkResult _CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    static void _DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        assert(func);
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }
    static VkDebugUtilsMessengerCreateInfoEXT _DebugMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = _vkDebugCallback;
        createInfo.pUserData = nullptr; // Optional
        return createInfo;
    }

    static void initVkDebugMessage(VkInstance instance)
    {
        assert(_ValidationLayer);

        VkDebugUtilsMessengerCreateInfoEXT createInfo = _DebugMessengerCreateInfo();

        if (_CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &_DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }








    ////////////// Physical Device & Logical Device //////////////

    // the arg VkSurfaceKHR is used to rate a device that whether supports Surface Present. 　
    static VkPhysicalDevice initPhysicalDevice(VkSurfaceKHR surface)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw std::runtime_error("failed to find GPU with vulkan support.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

        VkPhysicalDevice physicalDevice = nullptr;

        std::multimap<int, VkPhysicalDevice> candidates;
        for (const auto& device : devices) {
            int score = _DeviceSuitableRate(device, surface);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.cbegin()->first > 0) {
            physicalDevice = candidates.cbegin()->second;
        } else {
            throw std::runtime_error("failed to find a suitable GPU.");
        }

        return physicalDevice;
    }

    static int _DeviceSuitableRate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        QueueFamilyIndices queueFamily = _FindQueueFamilies(physicalDevice, surface);
        if (!queueFamily.isComplete())
            return 0;

        int score = 0;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 800;
        }
        score += deviceProperties.limits.maxImageDimension2D;

        Log::info("device ", deviceProperties.deviceName);
        return score;
    }



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

        if (queueFamilyCount == 0)
            return queueFamilyIdxs;

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());


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
            i++;
        }

        return queueFamilyIdxs;
    }



    static VkDevice initLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkQueue* _Dst_GraphicsQueue, VkQueue* _Dst_PresentQueue)
    {
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Queue Family
        QueueFamilyIndices queueFamily = _FindQueueFamilies(physicalDevice, surface);
        float queuePriority = 1.0f;  // 0.0-1.0

        int numQueues = 2;
        uint arrQueueFamilyIdx[] = {queueFamily.m_GraphicsFamily, queueFamily.m_PresentFamily};
        VkDeviceQueueCreateInfo arrQueueCreateInfo[numQueues];
        for (int i = 0;i < numQueues;++i) {
            VkDeviceQueueCreateInfo& queueCreateInfo = arrQueueCreateInfo[i];
            queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfo.queueFamilyIndex = arrQueueFamilyIdx[i];
            queueCreateInfo.queueCount = 1;
        }

        createInfo.pQueueCreateInfos = arrQueueCreateInfo;
        createInfo.queueCreateInfoCount = numQueues;

        // Device Features
        VkPhysicalDeviceFeatures deviceFeatures{};
        //vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        createInfo.pEnabledFeatures = &deviceFeatures;

//        const char* EXT[] = {
//                "VK_KHR_portability_subset"
//        };
//        createInfo.enabledExtensionCount = 1;
//        createInfo.ppEnabledExtensionNames = EXT;

        createInfo.enabledExtensionCount = 0;

        // already deprecated. ValidationLayer only belongs to VkInstance.
        if (_ValidationLayer) {
            createInfo.ppEnabledLayerNames = _ValidationLayers.data();
            createInfo.enabledLayerCount = _ValidationLayers.size();
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




    ///////////// SurfaceKHR /////////////

    static void initSurface(VkInstance vkInstance, GLFWwindow* glfwWindow, VkSurfaceKHR* _Dst_Surface)
    {

        VkResult vkErr = glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, _Dst_Surface);

        if (vkErr != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface via glfw.");
        }

    }
};

#endif //ETHERTIA_VULKAN_H
