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



    static void initVkInstance()
    {
        {
            uint32_t vkExtensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);
            std::cout << vkExtensionCount << " vk extensions supported\n";

            VkExtensionProperties vkExtProps[vkExtensionCount];
            vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtProps);

            for (int i = 0; i < vkExtensionCount; ++i) {
                std::cout << " - " << vkExtProps[i].extensionName << "\n";
            }
        }

        // Optional
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

        std::vector<const char*> extensionsRequired;
        for (int i = 0; i < glfwExtensionCount; ++i) {
            extensionsRequired.emplace_back(glfwExtensions[i]);
        }

        //{
        //    // for prevents VK_ERROR_INCOMPATIBLE_DRIVER err on MacOS MoltenVK sdk.
        //    extensionsRequired.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        //    vkCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        //}

        for (int i = 0; i < extensionsRequired.size(); ++i) {
            Log::info("EXT ", extensionsRequired[i]);
        }

        vkCreateInfo.enabledExtensionCount = extensionsRequired.size();
        vkCreateInfo.ppEnabledExtensionNames = extensionsRequired.data();
        vkCreateInfo.enabledLayerCount = 0;
        vkCreateInfo.pNext = nullptr;


        VkInstance vkInstance;
        VkResult vkErr = vkCreateInstance(&vkCreateInfo, nullptr, &vkInstance);
        if (vkErr) {
            throw std::runtime_error(Strings::fmt("failed to create vulkan instance. ", vkErr));
        }

    }

    void initVkDebugMessage() {

    }
};

#endif //ETHERTIA_VULKAN_H
