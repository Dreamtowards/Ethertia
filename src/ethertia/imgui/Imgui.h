//
// Created by Dreamtowards on 2023/3/3.
//

#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <ethertia/entity/Entity.h>
#include <ethertia/util/AABB.h>


class Imgui
{
public:
    inline static float GuiScale = 1.0f;

    inline static std::vector<std::string> g_MessageBox;
    inline static glm::vec4 wViewportXYWH;

    // selected entity in the Entity Inspector.
    inline static Entity* g_InspEntity = nullptr;

    static void Init();
    static void Destroy();

    static void NewFrame();
    static void Render(VkCommandBuffer cmdbuf);




    using DrawFuncPtr = void (*)(bool*);
    inline static std::vector<DrawFuncPtr> DrawFuncList;

    static void Show(DrawFuncPtr w);

    static bool Has(DrawFuncPtr w);

    static void Close(DrawFuncPtr w);

    static void ShowWindows();

    static void ToggleDrawCheckbox(const char* label, DrawFuncPtr w);



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

