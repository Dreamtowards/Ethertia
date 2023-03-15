//
// Created by Dreamtowards on 2023/3/3.
//

#ifndef ETHERTIA_IMGUIS_H
#define ETHERTIA_IMGUIS_H

//#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <ethertia/entity/Entity.h>


class ImGuis
{
public:
    inline static std::vector<std::string> g_MessageBox;

    static void Init();
    static void Destroy();

    static void RenderGUI();

    static void Image(GLuint texId, ImVec2 size, glm::vec4 color = {1,1,1,1});
    static ImVec2 GetWindowContentSize();

    // align: {0,0}=LeftTop, {1,1}=RightBottom, {0.5,0.5}=Center, etc.
    static void TextAlign(const char* text, ImVec2 align);



    static void forWorldpoint(const glm::vec3& worldpos, const std::function<void(glm::vec2)>& fn);
};

#endif //ETHERTIA_IMGUIS_H
