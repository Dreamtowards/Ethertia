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


    inline static VkImageView g_ComposeView = nullptr;
};

