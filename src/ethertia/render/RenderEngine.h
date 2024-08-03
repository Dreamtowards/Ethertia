//
// Created by Dreamtowards on 2022/4/22.
//

#pragma once

#include "glx.h"

class World;

class RenderEngine
{
public:
    RenderEngine() = delete;

    static void Init();
    static void Destroy();

    static void RenderWorld(World* world);

    inline static bool g_PauseWorldRender = false;
};

