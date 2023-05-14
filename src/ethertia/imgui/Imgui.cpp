//
// Created by Dreamtowards on 2023/3/8.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ethertia/imgui/Imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <imguizmo/ImGuizmo.h>
#include <imgui-knobs/imgui-knobs.h>
#include <imgui-imnodes/imnodes.h>


#include <ethertia/init/Settings.h>
#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/render/RenderEngine.h>

#include <ethertia/init/DebugStat.h>
#include <ethertia/init/Controls.h>

#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/world/Chunk.h>
//#include <ethertia/render/ssao/SSAORenderer.h>
//#include <ethertia/render/shadow/ShadowRenderer.h>
//#include <ethertia/render/debug/DebugRenderer.h>
//#include <ethertia/render/deferred/GeometryRenderer.h>

static void InitStyle()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConf;
    fontConf.OversampleH = 3;
    fontConf.OversampleV = 3;
    fontConf.RasterizerMultiply = 1.6f;
    io.Fonts->AddFontFromFileTTF("./assets/font/menlo.ttf", 14.0f, &fontConf);

    ImGuiStyle& styl = ImGui::GetStyle();
    styl.GrabMinSize = 7;
    styl.FrameBorderSize = 0;
    styl.WindowMenuButtonPosition = ImGuiDir_Right;
    styl.SeparatorTextBorderSize = 2;
    styl.DisplaySafeAreaPadding = {0, 0};
    styl.FramePadding = {8, 2};

    styl.ScrollbarSize = 10;
    styl.ScrollbarRounding = 2;
    styl.TabRounding = 2;

    for (int i = 0; i < ImGuiCol_COUNT; ++i)
    {
        ImVec4& col = styl.Colors[i];
        float f = std::max(Colors::luminance({col.x, col.y, col.z}), 0.06f);
        //(col.x + col.y + col.z) / 3.0f;
        col = ImVec4(f,f,f,col.w);
    }

    auto& Col = styl.Colors;

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



void Imgui::Init()
{
    BENCHMARK_TIMER;
    Log::info("Init ImGui.. \1");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    InitStyle();

    ImGuiIO& io = ImGui::GetIO();

    // Set Before Backend/Impl Init.
    io.ConfigFlags |=
            ImGuiConfigFlags_DockingEnable |           // Enable Docking.
//            ImGuiConfigFlags_ViewportsEnable;        // Multiple Windows/Viewports
//            ImGuiConfigFlags_DpiEnableScaleFonts |
//            ImGuiConfigFlags_DpiEnableScaleViewports;
//    io.ConfigViewportsNoDecoration = false;
//    io.ConfigViewportsNoTaskBarIcon = true;
//    ImGui::GetMainViewport()->DpiScale = 4;

//    ImGui_ImplGlfw_InitForOpenGL(Ethertia::getWindow().m_WindowHandle, true);
//    ImGui_ImplOpenGL3_Init("#version 150");  // GL 3.2 + GLSL 150

    ImGui_ImplGlfw_InitForVulkan(Ethertia::getWindow().m_WindowHandle, true);


    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vkx::ctx().Inst;
    init_info.PhysicalDevice = vkx::ctx().PhysDevice;
    init_info.Device = vkx::ctx().Device;
    init_info.Queue = vkx::ctx().GraphicsQueue;
    init_info.DescriptorPool = vkx::ctx().DescriptorPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = _vk_check_result;
    //init_info.QueueFamily = g_QueueFamily;
    //init_info.PipelineCache = g_PipelineCache;
    //init_info.Subpass = 0;
    //init_info.Allocator = g_Allocator;
    ImGui_ImplVulkan_Init(&init_info, vkx::ctx().MainRenderPass);


    vkx::SubmitCommandBuffer([](VkCommandBuffer cmdbuf)
    {
        ImGui_ImplVulkan_CreateFontsTexture(cmdbuf);
    });
    ImGui_ImplVulkan_DestroyFontUploadObjects();


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



void Imgui::NewFrame()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    ImGui_ImplGlfw_MousePosWindowScale = 1.0f / Imgui::GlobalScale;

    ImGui::GetMainViewport()->Size /= Imgui::GlobalScale;
    ImGui::GetIO().DisplayFramebufferScale *= Imgui::GlobalScale;
}


void Imgui::RenderGUI(VkCommandBuffer cmdbuf)
{
    {
        PROFILE("Render");

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdbuf);
    }

    // Update Multiple Windows/Viewports
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}




static VkDescriptorSet pTexDesc(VkImageView imageView)
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
void Imgui::Image(void* texId, ImVec2 size, glm::vec4 color) {
//    assert(false);
    if (texId == 0) {
        texId = pTexDesc(RenderEngine::TEX_WHITE->m_ImageView);
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
    glm::vec3 p = Mth::projectWorldpoint(worldpos, Ethertia::getCamera().matView, Ethertia::getCamera().matProjection);

    auto& vp = Ethertia::getViewport();
    out_screenpos.x = vp.x + p.x * vp.width;
    out_screenpos.y = vp.y + p.y * vp.height;

    return p.z > 0;
}

static ImGuiKey GetPressedKey()
{
    for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k)
    {
        if (ImGui::IsKeyPressed((ImGuiKey)k))
            return (ImGuiKey)k;
    }
    return ImGuiKey_None;
}





#include "Imgui_intl_draw.cpp"



void Imgui::RenderWindows()
{
    ShowDockspaceAndMainMenubar();

    if (Settings::w_Toolbar)
        ShowToolbar();

    ShowPlayerInventory();

    if (w_ImGuiDemo)
        ImGui::ShowDemoWindow(&w_ImGuiDemo);

    if (w_NewWorld)
        ShowNewWorldWindow();

    if (Settings::w_Profiler)
        ShowProfilers();

    if (Settings::w_EntityList)
        ShowEntities();
    if (Settings::w_EntityInsp) {
        ShowEntityInsp();
        if (Imgui::g_InspEntity) {
            Imgui::RenderAABB(Imgui::g_InspEntity->getAABB(), Colors::YELLOW);
        }
    }
    if (Settings::w_ShaderInsp) {
        ShowShaderProgramInsp();
    }

    if (Settings::w_Viewport)
    {
        ShowGameViewport();
    } else {
        Imgui::wViewportXYWH = {Mth::Inf, 0, 0, 0};
    }

    if (Settings::w_Console)
        ShowConsole();

    if (Settings::w_Settings) {
        ShowSettingsWindow();
    }



    if (w_NodeEditor)
        ShowNodeEditor();


    if (w_TitleScreen) {
        ShowTitleScreenWindow();
    }
    if (w_Singleplayer) {
        ShowSingleplayerWindow();
    }


}

