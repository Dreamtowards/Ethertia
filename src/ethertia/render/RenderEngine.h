//
// Created by Dreamtowards on 2022/4/22.
//

#pragma once

#include <vkx/vkx.hpp>


class RenderEngine
{
public:
    RenderEngine() = delete;

    static void Init();
    static void Destroy();

    static void Render();

    static void _ReloadPipeline();

    inline static bool s_PauseWorldRender = false;
};

