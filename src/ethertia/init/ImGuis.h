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

    inline static bool  g_ShowImGuiDemoWindow = false,
                        g_ShowNewWorldWindow = false,
                        g_ShowInspectorWindow = false,
                        g_ShowLoadedEntitiesWindow = false;


    inline static bool g_GizmoViewManipulation = true,
                       g_Game = true;

    inline static int g_WorldGrids = 24;

    inline static Entity* g_InspectorEntity = nullptr;

    static void Init();

    static void Destroy();

    static void ShowMainMenuBar();

    static void Render();

};

#endif //ETHERTIA_IMGUIS_H
