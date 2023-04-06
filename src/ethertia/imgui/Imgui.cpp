//
// Created by Dreamtowards on 2023/3/8.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ethertia/imgui/Imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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
#include <ethertia/render/ssao/SSAORenderer.h>
#include <ethertia/render/shadow/ShadowRenderer.h>
#include <ethertia/render/debug/DebugRenderer.h>
#include <ethertia/render/deferred/GeometryRenderer.h>

static void InitStyle()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontConf;
    fontConf.OversampleH = 3;
    fontConf.OversampleV = 3;
    fontConf.RasterizerMultiply = 1.6f;
    io.Fonts->AddFontFromFileTTF("./assets/font/menlo.ttf", 14.0f, &fontConf);


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

void Imgui::Init()
{
    BENCHMARK_TIMER;
    Log::info("Init ImGui.. \1");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Ethertia::getWindow().m_WindowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 150");  // GL 3.2 + GLSL 150

    InitStyle();

    ImNodes::CreateContext();
    ImNodes::GetIO().EmulateThreeButtonMouse.Modifier = &ImGui::GetIO().KeyShift;

}

void Imgui::Destroy()
{
    ImNodes::DestroyContext();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}







// texId: 0=white
void Imgui::Image(GLuint texId, ImVec2 size, glm::vec4 color) {
//    assert(false);
    if (texId == 0)
        texId = Texture::WHITE->texId;
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

void Imgui::forWorldpoint(const glm::vec3 &worldpos, const std::function<void(glm::vec2)> &fn)
{
    glm::vec3 p = Mth::projectWorldpoint(worldpos, Ethertia::getCamera().matView, Ethertia::getCamera().matProjection);

    auto& vp = Ethertia::getViewport();
    p.x = p.x * vp.width;
    p.y = p.y * vp.height;

    if (p.z > 0) {
        fn(glm::vec2(p.x, p.y));
    }
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


void Imgui::RenderGUI()
{
    {
        PROFILE("NewFrame");

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
    }

    {
        PROFILE("Proc");

        RenderWindows();
    }

    {
        PROFILE("Render");

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}