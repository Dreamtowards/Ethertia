//
// Created by Dreamtowards on 2023/3/19.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "Imgui.h"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

#include "VulkanIntl.h"

#include <array>



static VkDescriptorPool g_DescriptorPool_Imgui;


void Imgui::Init(GLFWwindow* glfwWindow)
{

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

    // tho oversize, but it's copied from imgui demo.
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER,               1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,         1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,         1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,  1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,  1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,        1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,        1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,      1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    auto& vk = VulkanIntl::GetState();

    VK_CHECK(vkCreateDescriptorPool(vk.g_Device, &pool_info, nullptr, &g_DescriptorPool_Imgui));


    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vk.g_Instance;
    init_info.PhysicalDevice = vk.g_PhysDevice;
    init_info.Device = vk.g_Device;
    init_info.Queue = vk.g_GraphicsQueue;
    init_info.DescriptorPool = g_DescriptorPool_Imgui;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = _vk_check_result;
    //init_info.QueueFamily = g_QueueFamily;
    //init_info.PipelineCache = g_PipelineCache;
    //init_info.Subpass = 0;
    //init_info.Allocator = g_Allocator;
    ImGui_ImplVulkan_Init(&init_info, vk.g_RenderPass);


    VulkanIntl::SubmitOnetimeCommandBuffer([](VkCommandBuffer cmdbuf)
    {
        ImGui_ImplVulkan_CreateFontsTexture(cmdbuf);
    });
}

void Imgui::Destroy()
{
    // wait vulkan idle

    vkDestroyDescriptorPool(VulkanIntl::GetState().g_Device, g_DescriptorPool_Imgui, nullptr);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
}

void Imgui::RenderGUI(VkCommandBuffer cmdbuf)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::ShowDemoWindow();


    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdbuf);
}