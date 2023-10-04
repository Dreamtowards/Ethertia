//
// Created by Dreamtowards on 2023/3/3.
//

#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <string>
#include <map>

#include <ethertia/util/AABB.h>


class Imgui
{
public:
    inline static float GuiScale = 1.0f;

    static void Init();
    static void Destroy();

    static void NewFrame();
    static void Render(VkCommandBuffer cmdbuf);




    using DrawFuncPtr = void (*)(bool*);
    inline static std::vector<DrawFuncPtr> DrawFuncList;

    // for Store & Restore opened windows.
    inline static std::map<DrawFuncPtr, std::string> DrawFuncIds;

    static void Show(DrawFuncPtr w);

    static bool Has(DrawFuncPtr w);

    static void Close(DrawFuncPtr w);

    static void ShowWindows();

    static void MenuItemToggleShow(const char* label, DrawFuncPtr w, const char* shortcut = nullptr, bool enabled = true);

    static void ToggleShow(DrawFuncPtr w);




    static void ItemTooltip(const std::string& str);

    static bool InputText(const char* label, std::string& text, const char* hint = nullptr);



    static VkDescriptorSet mapImage(VkImageView imageView);

    static void Image(void* texId, ImVec2 size, glm::vec4 color = {1,1,1,1});
    static ImVec2 GetWindowContentSize();

    // align: {0,0}=LeftTop, {1,1}=RightBottom, {0.5,0.5}=Center, etc.
    static void TextAlign(const char* text, ImVec2 align);



    // return true: visible (not in back / out of view)
    static bool CalcViewportWorldpos(glm::vec3 worldpos, glm::vec2& out_screenpos);

    static void RenderWorldLine(glm::vec3 p0, glm::vec3 p1, ImU32 col = ImGui::GetColorU32({1,1,1,1}), float thickness = 1.0f);

    static void RenderAABB(glm::vec3 min, glm::vec3 max, ImU32 col, float tk = 1.0f);

    static void RenderAABB(const AABB& aabb, glm::vec4 col);
};



/*

ChangeLogs to imgui

1. Gui Scale


2. smaller arrow on ImGui::RenderArrow, imgui_draw.cpp 
https://github.com/ocornut/imgui/issues/2235#issuecomment-602231017
    ...
    case ImGuiDir_Down:
        if (dir == ImGuiDir_Up) r = -r;
        a = ImVec2(+0.000f, +0.450f) * r;
        b = ImVec2(-0.800f, -0.450f) * r;
        c = ImVec2(+0.800f, -0.450f) * r;
        break;
    case ImGuiDir_Left:
    case ImGuiDir_Right:
        if (dir == ImGuiDir_Left) r = -r;
        a = ImVec2(+0.450f, +0.000f) * r;
        b = ImVec2(-0.450f, +0.800f) * r;
        c = ImVec2(-0.450f, -0.800f) * r;
        break;


*/