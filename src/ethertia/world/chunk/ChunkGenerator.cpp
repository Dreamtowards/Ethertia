
#include "ChunkGenerator.h"

#include <ethertia/world/Chunk.h>
#include <ethertia/util/NoiseGen.h>


inline static int IdxXZ(int rx, int rz) {
    return rz * 16 + rx;
}
inline static int Idx3(int rx, int ry, int rz) {
    return rz * 256 + ry * 16 + rx;
}

void ChunkGenerator::GenerateChunk(Chunk& chunk)
{
    uint64_t seed = chunk.GetWorld().GetSeed();

    glm::ivec3 chunkpos = chunk.chunkpos;

    auto FBM = NoiseGen::NewFractalFBM();
    FBM->SetSource(NoiseGen::Simplex());
    FBM->SetOctaveCount(5);

    float noise2d[16 * 16];
    FBM->GenUniformGrid2D(noise2d, chunkpos.x, chunkpos.z, 16, 16, 1 / 400.0f, seed);

    for (int dx = 0; dx < 16; ++dx)
    {
        for (int dy = 0; dy < 16; ++dy)
        {
            for (int dz = 0; dz < 16; ++dz)
            {
                glm::ivec3 lp{ dx, dy, dz };
                glm::ivec3 p = lp + chunkpos;

                float terr2d = noise2d[IdxXZ(dx, dz)];

                float f = terr2d - p.y / 30.0f;

                
                
                Material* mtl = nullptr;
                if (f > 0)
                {
                }

                Cell& cell = chunk.LocalCell(lp);

                cell.value = f;
            }
        }
    }

}


void ChunkGenerator::PopulateChunk(Chunk& chunk)
{

}