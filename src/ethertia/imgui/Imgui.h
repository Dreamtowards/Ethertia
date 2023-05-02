//
// Created by Dreamtowards on 2023/3/3.
//

#ifndef ETHERTIA_IMGUIS_H
#define ETHERTIA_IMGUIS_H

//#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <ethertia/entity/Entity.h>
#include <ethertia/util/AABB.h>


class Imgui
{
public:
    inline static float GlobalScale = 1.0f;

    inline static std::vector<std::string> g_MessageBox;
    inline static glm::vec4 wViewportXYWH;

    // selected entity in the Entity Inspector.
    inline static Entity* g_InspEntity = nullptr;

    static void Init();
    static void Destroy();

    static void RenderGUI(VkCommandBuffer cmdbuf);
    static void NewFrame();

    static void RenderWindows();


    static void Image(void* texId, ImVec2 size, glm::vec4 color = {1,1,1,1});
    static ImVec2 GetWindowContentSize();

    // align: {0,0}=LeftTop, {1,1}=RightBottom, {0.5,0.5}=Center, etc.
    static void TextAlign(const char* text, ImVec2 align);



    static void forWorldpoint(const glm::vec3& worldpos, const std::function<void(glm::vec2)>& fn);

    // return true: visible (not in back / out of view)
    static bool CalcViewportWorldpos(glm::vec3 worldpos, glm::vec2& out_screenpos);

    static void RenderWorldLine(glm::vec3 p0, glm::vec3 p1, ImU32 col = ImGui::GetColorU32({1,1,1,1}), float thickness = 1.0f);

    static void RenderAABB(glm::vec3 min, glm::vec3 max, ImU32 col, float tk = 1.0f);

    static void RenderAABB(const AABB& aabb, glm::vec4 col);
};

#endif //ETHERTIA_IMGUIS_H
