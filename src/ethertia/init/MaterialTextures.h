//
// Created by Dreamtowards on 2022/9/11.
//
// Ethernet
// Etherpedia
// Etheream   Etheream
// Etherald
// Etheria

#pragma once

#include <ethertia/material/Materials.h>


class MaterialTextures
{
public:
    // 1024 elaborated
    // 512  pretty realistic
    // 256  pretty
    // 128  non-realistic
    inline static int TEX_RESOLUTION = 256;

    inline static vkx::Image* ATLAS_DIFFUSE = nullptr;
    inline static vkx::Image* ATLAS_NORM    = nullptr;
    inline static vkx::Image* ATLAS_DRAM    = nullptr;  // DRAM: Disp, Rough, AO, Metal


    static void Load();

    static void Destroy()
    {
        delete ATLAS_DIFFUSE;
        delete ATLAS_NORM;
        delete ATLAS_DRAM;
    }

    static glm::vec2 uv_add(int mtlId) {
        return {(float)mtlId / Material::REGISTRY.size(), 0.0f};
    }
    static glm::vec2 uv_mul() {
        return {1.0f / Material::REGISTRY.size(), 1.0f};
    }

    static glm::vec2 uv_to_atlas_region(glm::vec2 uv, int mtlId) {
        float cap = Material::REGISTRY.size();
        return {
            (float)mtlId/cap + uv.x/cap,
            uv.y
        };
    }

};

