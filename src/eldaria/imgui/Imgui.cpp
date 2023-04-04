//
// Created by Dreamtowards on 2023/3/19.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "Imgui.h"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

#include <eldaria/vulkan/VulkanIntl.h>

#include <array>
#include <vector>

#include <ethertia/util/Colors.h>

static void InitStyle()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConf;
    fontConf.OversampleH = 3;
    fontConf.OversampleV = 3;
    fontConf.RasterizerMultiply = 1.6f;
    // fontConf.GlyphExtraSpacing.x = 1.0f;
    io.Fonts->AddFontFromFileTTF("./assets/font/googlesans.ttf", 14.0f, &fontConf);

    // Enable Docking.
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    style.GrabMinSize = 7;
    style.FrameBorderSize = 0;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.SeparatorTextBorderSize = 2;
    style.DisplaySafeAreaPadding = {0, 0};
    style.FramePadding = {8, 2};

    style.ScrollbarSize = 10;
    style.ScrollbarRounding = 2;
    style.TabRounding = 2;

    for (int i = 0; i < ImGuiCol_COUNT; ++i)
    {
        ImVec4& col = style.Colors[i];
        float f = std::max(Colors::luminance({col.x, col.y, col.z}), 0.06f);
        //(col.x + col.y + col.z) / 3.0f;
        col = ImVec4(f,f,f,col.w);
    }

    auto& Col = style.Colors;

    Col[ImGuiCol_CheckMark] =
    Col[ImGuiCol_SliderGrab] =
            {1.000f, 1.000f, 1.000f, 1.000f};

//    style.Colors[ImGuiCol_MenuBarBg] = {0,0,0,0};

    Col[ImGuiCol_HeaderHovered] = {0.051f, 0.431f, 0.992f, 1.000f};
    Col[ImGuiCol_HeaderActive] = {0.071f, 0.388f, 0.853f, 1.000f};
    Col[ImGuiCol_Header] = {0.106f, 0.298f, 0.789f, 1.000f};  // also for Selectable.

    Col[ImGuiCol_TitleBg] = {0.082f, 0.082f, 0.082f, 0.800f};
    Col[ImGuiCol_TitleBgActive] = {0.082f, 0.082f, 0.082f, 1.000f};

    Col[ImGuiCol_Tab] =
    Col[ImGuiCol_TabUnfocused] = {0,0,0,0};

    Col[ImGuiCol_TabActive] = {0.26f, 0.26f, 0.26f, 1.000f};
    Col[ImGuiCol_TabUnfocusedActive] =
    Col[ImGuiCol_WindowBg] = {0.176f, 0.176f, 0.176f, 1.000f}; //{0.19f, 0.19f, 0.19f, 1.0f};  // {0.212f, 0.212f, 0.212f, 1.000f};
    Col[ImGuiCol_TitleBg] =
    Col[ImGuiCol_TitleBgActive] ={0.128f, 0.128f, 0.128f, 0.940f};

//        style.Colors[ImGuiCol_TitleBg] = {0.297f, 0.297f, 0.298f, 1.000f};
//        style.Colors[ImGuiCol_Button] =
//        style.Colors[ImGuiCol_Header] =
//        style.Colors[ImGuiCol_FrameBg] =
//                {0.322f, 0.322f, 0.322f, 0.540f};
//
//        style.Colors[ImGuiCol_ButtonHovered] =
//        style.Colors[ImGuiCol_HeaderHovered] =
//        style.Colors[ImGuiCol_FrameBgHovered] =
//                {0.626f, 0.626f, 0.626f, 0.400f};
//
//        style.Colors[ImGuiCol_ButtonActive] =
//        style.Colors[ImGuiCol_HeaderActive] =
//                {0.170f, 0.170f, 0.170f, 1.000f};

}




static VkDescriptorPool g_DescriptorPool_Imgui = nullptr;


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


    InitStyle();


    VulkanIntl::SubmitOnetimeCommandBuffer([](VkCommandBuffer cmdbuf)
    {
        ImGui_ImplVulkan_CreateFontsTexture(cmdbuf);
    });
    ImGui_ImplVulkan_DestroyFontUploadObjects();

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

#include <eldaria/init/DebugStat.h>

#include "Imgui_Intl_Impl.cpp"



static void ShowViewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::Begin("Viewport", &Imgui::w_Viewport);
    ImGui::PopStyleVar();

//    ImVec2 size = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
//    ImVec2 pos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
//    ImGuis::wViewportXYWH = {pos.x, pos.y, size.x, size.y};

    static void* texId = ImGui_ImplVulkan_AddTexture(VulkanIntl::GetState().g_TextureSampler,
                                                     VulkanIntl::getTestImgView(),
                                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    ImGui::Image(texId, ImGui::GetContentRegionAvail());
    ImGui::SetCursorPos({0,0});
//    ImGui::InvisibleButton("PreventsGameWindowMove", size);


//    ImGuizmo::BeginFrame();
//    ImGuizmo::SetOrthographic(false);
//    ImGuizmo::SetDrawlist();
//    auto& vp = Ethertia::getViewport();
//    ImGuizmo::SetRect(vp.x, vp.y, vp.width, vp.height);
//
//    if (g_WorldGrids > 0)
//    {
//        glm::mat4 iden(1.0f);
//        ImGuizmo::DrawGrid(glm::value_ptr(Ethertia::getCamera().matView), glm::value_ptr(Ethertia::getCamera().matProjection),
//                           glm::value_ptr(iden), (float)g_WorldGrids);
//    }
//
//    if (g_GizmoViewManipulation)
//    {
//        static float camLen = 10.0f;
//        auto& vp = Ethertia::getViewport();
//        ImGuizmo::ViewManipulate(glm::value_ptr(Ethertia::getCamera().matView), camLen,
//                                 ImVec2(vp.right()-128-24, vp.y+24), ImVec2(128, 128),
//                                 0x10101010);
//    }

    ImGui::End();
}


static void DrawWindows()
{
    ShowDockspaceAndMainMenubar();

    if (Imgui::w_Viewport) {
        ShowViewport();
    }

    if (Imgui::w_Settings) {
        ShowSettingsWindow();
    }

    // Draw Debug
    if (Imgui::w_ImguiDemo) {
        ImGui::ShowDemoWindow(&Imgui::w_ImguiDemo);
    }

}


void Imgui::RenderGUI(VkCommandBuffer cmdbuf)
{
    if (Dbg::dbg_PauseImgui)
        return;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    DrawWindows();


    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdbuf);
}