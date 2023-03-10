//
// Created by Dreamtowards on 2023/3/3.
//

#ifndef ETHERTIA_IMGUIS_H
#define ETHERTIA_IMGUIS_H


#include <imgui.h>

#include <ethertia/entity/Entity.h>


class ImGuis
{
public:

    inline static bool  g_ShowImGuiDemo = false,
                        g_ShowNewWorld = false,
                        g_ShowEntityInsp = false,
                        g_ShowLoadedEntities = false,
                        g_ShowShaderProgramInsp;


    inline static bool g_GizmoViewManipulation = true,
                       g_Game = true;

    inline static int g_WorldGrids = 24;

    inline static Entity* g_InspectorEntity = nullptr;

    inline static ShaderProgram* g_InspShaderProgram = nullptr;

    static void Init();

    static void Destroy();

    static void ShowMainMenuBar();

    static void Render();

};

#endif //ETHERTIA_IMGUIS_H
