//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKRENDERER_H
#define ETHERTIA_CHUNKRENDERER_H

#include <ethertia/client/render/shader/ShaderProgram.h>
#include <ethertia/client/Loader.h>
#include <ethertia/world/chunk/Chunk.h>

class ChunkRenderer {

    ShaderProgram shader{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh")};

public:

    void render(Chunk* chunk);

};

#endif //ETHERTIA_CHUNKRENDERER_H
