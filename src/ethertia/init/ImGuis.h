//
// Created by Dreamtowards on 2023/3/3.
//

#ifndef ETHERTIA_IMGUIS_H
#define ETHERTIA_IMGUIS_H



# define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <ethertia/entity/Entity.h>


class ImGuis
{
public:

    inline static bool  g_ShowImGuiDemo = false,
                        g_ShowNewWorld = false,
                        g_ShowEntityInsp = false,
                        g_ShowLoadedEntities = false,
                        g_ShowShaderProgramInsp = false,
                        g_ShowMessageBox = false,
                        g_ShowNodeEditor = false,
                        dbg_Text = false,
                        dbg_ViewBasis = false,
                        dbg_WorldBasis = false,
                        dbg_AllEntityAABB = false,
                        dbg_AllChunkAABB = false,
                        dbg_Gbuffer = false;

    inline static std::vector<std::string> g_MessageBox;


    inline static bool g_GizmoViewManipulation = true,
                       g_Game = false;

    inline static int g_WorldGrids = 10;

    inline static Entity* g_InspectorEntity = nullptr;

    inline static ShaderProgram* g_InspShaderProgram = nullptr;

    static void Init();

    static void Destroy();

    static void ShowMainMenuBar();

    static void Render();

    static void InnerRender();

};

#endif //ETHERTIA_IMGUIS_H
