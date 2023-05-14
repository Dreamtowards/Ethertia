//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <FastNoise/FastNoise.h>
#include <ethertia/world/gen/NoiseGen.h>

#include <ethertia/material/Materials.h>
#include <ethertia/world/Cell.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/util/Log.h>


class ChunkGenerator
{
public:
    using vec3 = glm::vec3;

    virtual void GenerateChunk(Chunk* chunk)  = 0;

    virtual void PopulateChunk(Chunk* chunk) {}

};

#endif //ETHERTIA_CHUNKGENERATOR_H
