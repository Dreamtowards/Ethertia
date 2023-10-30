//
// Created by Dreamtowards on 2023/3/8.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include "Imgui.h"
#include <ImGuizmo.h>
#include <imgui-knobs.h>
#include <imnodes.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <vkx/vkx.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/util/Colors.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>


static void InitStyle()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConf;
    fontConf.OversampleH = 2;
    fontConf.OversampleV = 2;
    fontConf.RasterizerMultiply = 1.6f;
    io.Fonts->AddFontFromFileTTF("./assets/font/menlo.ttf", 14.0f, &fontConf);

    ImGuiStyle& styl = ImGui::GetStyle();
    styl.GrabMinSize = 7;
    styl.FrameBorderSize = 0;
    styl.WindowMenuButtonPosition = ImGuiDir_Right;
    styl.SeparatorTextBorderSize = 1;
    styl.DisplaySafeAreaPadding = {0, 0};
    styl.WindowPadding = { 6, 6 };

    styl.ScrollbarSize = 10;
    styl.ScrollbarRounding = 2;
    styl.TabBarBorderSize = 1;
    styl.TabRounding = 2;
    styl.FramePadding = { 8, 2 };
    styl.FrameRounding = 2;


    auto& Col = styl.Colors;




    //Col[ImGuiCol_MenuBarBg] = {0,0,0,0};
    Col[ImGuiCol_Tab] =
    Col[ImGuiCol_TabUnfocused] = {0,0,0,0};
    Col[ImGuiCol_TabHovered] = { 0.377f, 0.377f, 0.398f, 0.800f };

    Col[ImGuiCol_TabUnfocusedActive] =
    Col[ImGuiCol_WindowBg] = { 0.19f, 0.19f, 0.19f, 1.0f }; //{0.19f, 0.19f, 0.19f, 1.0f};  // {0.212f, 0.212f, 0.212f, 1.000f};
    

    // Darker Dockspace Border. not working anymore, use Border instead
    //Col[ImGuiCol_Separator] = { 0.000f, 0.000f, 0.000f, 0.500f };
    //Col[ImGuiCol_SeparatorHovered] = {0.117f, 0.117f, 0.117f, 0.780f};
    //Col[ImGuiCol_SeparatorActive] =  {0.216f, 0.216f, 0.216f, 1.000f};

    //Col[ImGuiCol_TitleBg] = {0.297f, 0.297f, 0.298f, 1.000f};
    //Col[ImGuiCol_Button] =
    //Col[ImGuiCol_Header] =
    //Col[ImGuiCol_FrameBg] =
    //        {0.322f, 0.322f, 0.322f, 0.540f};
    //
    //Col[ImGuiCol_ButtonHovered] =
    //Col[ImGuiCol_HeaderHovered] =
    //Col[ImGuiCol_FrameBgHovered] =
    //        {0.626f, 0.626f, 0.626f, 0.400f};
    //
    //Col[ImGuiCol_ButtonActive] =
    //Col[ImGuiCol_HeaderActive] =
    //        {0.170f, 0.170f, 0.170f, 1.000f};


    for (int i = 0; i < ImGuiCol_COUNT; ++i)
    {
        ImVec4& col = styl.Colors[i];
        float f = std::max(Colors::luminance({ col.x, col.y, col.z }), 0.06f);
        if (f < 0.5) f *= 0.9;
        col = ImVec4(f, f, f*1.04f, col.w);
    }

    Col[ImGuiCol_HeaderHovered] = { 0.051f, 0.431f, 0.992f, 1.000f };
    Col[ImGuiCol_HeaderActive] = { 0.071f, 0.388f, 0.853f, 1.000f };


    Col[ImGuiCol_Border]  = { 0.090f, 0.090f, 0.094f, 1.000f };


    Col[ImGuiCol_TabActive] = { 0.234f, 0.234f, 0.243f, 1.000f };  // (0.251f, 0.251f, 0.255f, 1.000f)
    Col[ImGuiCol_PopupBg] = { 0.123f, 0.123f, 0.123f, 0.940f };

    Col[ImGuiCol_TitleBg] = { 0.131f, 0.135f, 0.135f, 1.000f }; // a bit lighter, diff from border.  // old: 0.082f, 0.082f, 0.082f, 0.800f
    Col[ImGuiCol_MenuBarBg] =
    Col[ImGuiCol_TitleBgActive] = { 0.118f, 0.118f, 0.119f, 1.000f };

    // darker: (0.000f, 0.000f, 0.000f, 0.351f)
    // gray: (0.323f, 0.333f, 0.357f, 0.596f)
    Col[ImGuiCol_Header] = { 0.106f, 0.298f, 0.789f, 1.000f };  // also for Selectable.

    Col[ImGuiCol_CheckMark] =
    Col[ImGuiCol_SliderGrab] =
        { 1.000f, 1.000f, 1.000f, 1.000f };
}


static void ImplImgui_VulkanCheckResult(VkResult r)
{
    if (r != VK_SUCCESS) {
        Log::warn("ImGui Vulkan Error");
    }
    vkx::check(r);
}

static void InitForVulkan()
{
    ImGui_ImplGlfw_InitForVulkan(Window::Handle(), true);

    VKX_CTX_device_allocator;
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = vkxc.Instance;
    initInfo.PhysicalDevice = vkxc.PhysDevice;
    initInfo.Device = vkxc.Device;
    initInfo.QueueFamily = vkxc.QueueFamily.GraphicsFamily;
    initInfo.Queue = vkxc.GraphicsQueue;
    initInfo.PipelineCache = nullptr;
    initInfo.DescriptorPool = vkxc.DescriptorPool;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = vkxc.SwapchainImages.size();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = (VkAllocationCallbacks*)vkxc.Allocator;
    initInfo.CheckVkResultFn = ImplImgui_VulkanCheckResult;

    // Dynamic Rendering, VK_KHR_dynamic_rendering
    //initInfo.UseDynamicRendering = false;
    //initInfo.ColorAttachmentFormat = ;

    ImGui_ImplVulkan_Init(&initInfo, vkxc.MainRenderPass);

    vkx::SubmitCommandBuffer([](vk::CommandBuffer cmdbuf)
    {
        ImGui_ImplVulkan_CreateFontsTexture(cmdbuf);
    });
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}



void Imgui::Init()
{
    BENCHMARK_TIMER;
    Log::info("ImGui initializing.. \1");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    InitStyle();

    ImGuiIO& io = ImGui::GetIO();

    // Set Before Backend/Impl Init.
    io.ConfigFlags |=
            ImGuiConfigFlags_DockingEnable |         // Enable Docking.
            ImGuiConfigFlags_ViewportsEnable;        // Multiple Windows/Viewports
//            ImGuiConfigFlags_DpiEnableScaleFonts |
//            ImGuiConfigFlags_DpiEnableScaleViewports;
//    io.ConfigViewportsNoDecoration = false;
//    io.ConfigViewportsNoTaskBarIcon = true;
//    ImGui::GetMainViewport()->DpiScale = 4;


    InitForVulkan();

    // ImNodes
    ImNodes::CreateContext();
    ImNodes::GetIO().EmulateThreeButtonMouse.Modifier = &ImGui::GetIO().KeyShift;

}

void Imgui::Destroy()
{
    ImNodes::DestroyContext();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


/*

DockSpace
  DockNodeUpdate
    DockNodeUpdateTabBar :16850
      DockNodeWindowMenuUpdate :16732 Popup
        DockNodeWindowMenuHandler 
  

*/

void Imgui::NewFrame()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    // Mouse Doesn't work anymore. MousePos is in ViewportSize coordinate.
    //ImGui_EtPatch_MousePosScale = Imgui::GuiScale;

    //ImGui::GetMainViewport()->Size *= 1.0f / Imgui::GuiScale;  // Viewport LogicSize. *= invScale
    //ImGui::GetIO().DisplayFramebufferScale *= Imgui::GuiScale;  // Framebuffer RenderSize


    ImGuizmo::SetOrthographic(false);
    //ImGuizmo::SetOrthographic(Ethertia::GetCamera().IsOrthographic());
    ImGuizmo::BeginFrame();

    auto vp = Ethertia::GetViewport();
    ImGuizmo::SetRect(vp.x, vp.y, vp.width, vp.height);

}


void Imgui::Render(VkCommandBuffer cmdbuf)
{
    {
        ET_PROFILE("Render");

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdbuf);
    }

    // Update Multiple Windows/Viewports
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}






#include <ethertia/init/Settings.h>

#include <stdx/collection.h>


void Imgui::Show(DrawFuncPtr w)
{
    if (Has(w))
    {
        Log::info("Failed to ShowWindow, Existed Already");
        return;
    }
    Imgui::DrawFuncList.push_back(w);
}

bool Imgui::Has(DrawFuncPtr w)
{
    return stdx::exists(Imgui::DrawFuncList, w);  //auto& ls = ImWindows::Windows;std::find(ls.begin(), ls.end(), w) != ls.end();
}

void Imgui::Close(DrawFuncPtr w)
{
    stdx::erase(Imgui::DrawFuncList, w);
}

void Imgui::ShowWindows()
{
    auto& windows = Imgui::DrawFuncList;
    for (int i = windows.size()-1; i >= 0; --i)
    {
        bool show = true;

        {
            ET_PROFILE(Imgui::DrawFuncIds[windows[i]].c_str());

            windows[i](&show);
        }

        if (!show)
        {
            stdx::erase(windows, i);
        }
    }
}

void Imgui::MenuItemToggleShow(const char* label, DrawFuncPtr w, const char* shortcut, bool enabled)
{
    bool show = Imgui::Has(w);
    if (ImGui::MenuItem(label, shortcut, show, enabled))
    {
        if (show) {
            Imgui::Close(w);
        } else {
            Imgui::Show(w);
        }
    }
}


void Imgui::ToggleShow(DrawFuncPtr w)
{
    if (Imgui::Has(w)) {
        Imgui::Close(w);
    } else {
        Imgui::Show(w);
    }
}






void Imgui::ItemTooltip(const std::string& str)
{
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
    {
        ImGui::BeginTooltip();
        ImGui::TextDisabled(str.c_str());
        ImGui::EndTooltip();
    }
}


bool Imgui::InputText(const char* label, std::string& text, const char* hint)
{
    static char buffer[256];
    strncpy(buffer, text.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;

    if (ImGui::InputTextWithHint(label, hint, buffer, sizeof(buffer)))
    {
        text = buffer;
        return true;
    }
    else
    {
        return false;
    }
}



VkDescriptorSet Imgui::mapImage(VkImageView imageView)
{
    static std::unordered_map<VkImageView, VkDescriptorSet> _cache;

    auto it = _cache.find(imageView);
    if (it == _cache.end()) {
        _cache[imageView] = ImGui_ImplVulkan_AddTexture(vkx::ctx().ImageSampler,
                                                        imageView,
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    return _cache[imageView];
}




// texId: 0=white
void Imgui::Image(void* texId, ImVec2 size, glm::vec4 color) 
{
    if (texId == 0) {
        throw 4;
        //texId = pTexDesc(RenderEngine::TEX_WHITE->imageView);
    }
    ImGui::Image((void*)(intptr_t)texId,
                 {size.x, size.y},
                 {0, 1}, {1, 0},
                 {color.x, color.y, color.z, color.w});
}

ImVec2 Imgui::GetWindowContentSize() {
    return ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
}

void Imgui::TextAlign(const char* text, ImVec2 align) {
    ImVec2 size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - align.x * size.x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - align.y * size.y);
    ImGui::Text("%s", text);
}

bool Imgui::CalcViewportWorldpos(glm::vec3 worldpos, glm::vec2& out_screenpos)
{
    auto& cam = Ethertia::GetCamera();
    glm::vec3 p = Mth::projectWorldpoint(worldpos, cam.matView, cam.matProjection);

    auto vp = Ethertia::GetViewport();
    out_screenpos.x = vp.x + p.x * vp.width;
    out_screenpos.y = vp.y + p.y * vp.height;

    return p.z > 0;
}



