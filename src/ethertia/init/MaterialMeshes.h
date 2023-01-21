//
// Created by Dreamtowards on 2023/1/21.
//

#ifndef ETHERTIA_MATERIALMESHES_H
#define ETHERTIA_MATERIALMESHES_H

#include <ethertia/render/VertexBuffer.h>
#include <ethertia/util/Loader.h>

class MaterialMeshes
{
public:

    inline static const VertexBuffer* STOOL = nullptr;

    static void load()
    {
        BENCHMARK_TIMER;
        Log::info("Loading material meshes...\1");

        STOOL = Loader::loadOBJ_("material/material_mesh/stool/mesh.obj");


    }

};

#endif //ETHERTIA_MATERIALMESHES_H
