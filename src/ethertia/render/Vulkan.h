//
// Created by Dreamtowards on 2023/1/20.
//

#ifndef ETHERTIA_VULKAN_H
#define ETHERTIA_VULKAN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>



class Vulkan
{
public:
    inline static bool _ValidationLayer = true;
    inline static std::vector<const char*> _ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    inline static VkDebugUtilsMessengerEXT _DebugMessenger = nullptr;

    inline static VkInstance vkInstance = nullptr;

    static void init()
    {
        initVkInstance();

        if (_ValidationLayer) {
            initVkDebugMessage();
        }
    }

    static void deinit()
    {
        if (_ValidationLayer) {
            _DestroyDebugUtilsMessengerEXT(vkInstance, _DebugMessenger, nullptr);
        }

        vkDestroyInstance(vkInstance, nullptr);
    }


private:

    static void initVkInstance()
    {
//        {
//            uint32_t vkExtensionCount = 0;
//            vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);
//            std::cout << vkExtensionCount << " vk extensions supported\n";
//
//            VkExtensionProperties vkExtProps[vkExtensionCount];
//            vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtProps);
//
//            for (int i = 0; i < vkExtensionCount; ++i) {
//                std::cout << " - " << vkExtProps[i].extensionName << "\n";
//            }
//        }

        // Optional Info.
        VkApplicationInfo vkAppInfo{};
        vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.pApplicationName = "TestApp";
        vkAppInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 1);
        vkAppInfo.pEngineName = "No Engine";
        vkAppInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        vkAppInfo.apiVersion = VK_API_VERSION_1_0;


        VkInstanceCreateInfo vkCreateInfo{};
        vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkCreateInfo.pApplicationInfo = &vkAppInfo;


        // Global Extensions (not spec device.)
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensionsRequired;//, glfwExtensions + glfwExtensionCount);
        for (int i = 0; i < glfwExtensionCount; ++i) {
            extensionsRequired.emplace_back(glfwExtensions[i]);
        }

        if (_ValidationLayer) {
            extensionsRequired.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        {
            // for prevents VK_ERROR_INCOMPATIBLE_DRIVER err on MacOS MoltenVK sdk.
            extensionsRequired.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            vkCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        Log::info("Global extensions: \1");
        for (int i = 0; i < extensionsRequired.size(); ++i) {
            std::cout << extensionsRequired[i] << ", ";
        }
        std::cout << "\n";

        vkCreateInfo.enabledExtensionCount = extensionsRequired.size();
        vkCreateInfo.ppEnabledExtensionNames = extensionsRequired.data();


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

        VkResult vkErr = vkCreateInstance(&vkCreateInfo, nullptr, &vkInstance);
        if (vkErr) {
            throw std::runtime_error(Strings::fmt("failed to create vulkan instance. ", vkErr));
        }
    }


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
                throw std::runtime_error(Strings::fmt("required validation layer not available: {} of available {}: {}", layerName, layerCount, availableLayers[0].layerName));
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

    static void initVkDebugMessage()
    {
        assert(_ValidationLayer);

        VkDebugUtilsMessengerCreateInfoEXT createInfo = _DebugMessengerCreateInfo();

        if (_CreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &_DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
};

#endif //ETHERTIA_VULKAN_H
