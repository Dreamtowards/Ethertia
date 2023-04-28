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

    inline static const VertexData* CAPSULE = nullptr;
    inline static const VertexData* STOOL = nullptr;
    inline static const VertexData* CASH_REGISTER = nullptr;
    inline static const VertexData* SHORT_GRASS = nullptr;



    static void load()
    {
        BENCHMARK_TIMER;
        Log::info("Loading material meshes...\1");

        CAPSULE = Loader::loadOBJ("entity/capsule-1-2.obj");

        for (auto& it : Material::REGISTRY)
        {
            const std::string& id = it.first;
            Material* mtl = it.second;

            if (mtl->m_CustomMesh)
            {
                mtl->m_VertexBuffer = Loader::loadOBJ(Strings::fmt("material/{}/mesh.obj", id).c_str());
            }
        }
    }

};

#endif //ETHERTIA_MATERIALMESHES_H
