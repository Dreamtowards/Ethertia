//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKMESHGEN_H
#define ETHERTIA_CHUNKMESHGEN_H

#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/client/Loader.h>
#include <ethertia/client/render/TextureAtlas.h>
#include <ethertia/world/World.h>
#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/init/Blocks.h>
#include <ethertia/init/BlockTextures.h>

class ChunkMeshGen
{
public:

    // Winding:
    // [2,4] +---+ [1]
    //       | \ |
    //   [5] +---+ [0,3]
    // Faces: -X, +X, -Y, +Y, -Z, +Z.
    inline static float CUBE_POS[] = {
            0, 0, 1, 0, 1, 1, 0, 1, 0,  // Left -X
            0, 0, 1, 0, 1, 0, 0, 0, 0,
            1, 0, 0, 1, 1, 0, 1, 1, 1,  // Right +X
            1, 0, 0, 1, 1, 1, 1, 0, 1,
            0, 0, 1, 0, 0, 0, 1, 0, 0,  // Bottom -Y
            0, 0, 1, 1, 0, 0, 1, 0, 1,
            0, 1, 1, 1, 1, 1, 1, 1, 0,  // Bottom +Y
            0, 1, 1, 1, 1, 0, 0, 1, 0,
            0, 0, 0, 0, 1, 0, 1, 1, 0,  // Front -Z
            0, 0, 0, 1, 1, 0, 1, 0, 0,
            1, 0, 1, 1, 1, 1, 0, 1, 1,  // Back +Z
            1, 0, 1, 0, 1, 1, 0, 0, 1,
    };
    inline static float CUBE_UV[] = {
            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,  // One Face.
            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
    };
    inline static float CUBE_NORM[] = {
           -1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,
            1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
            0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0,
            0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
            0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1,
            0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1
    };


    // invoke from ChunkGen thread.
    static VertexBuffer* genMesh(Chunk* chunk, World* world) {
        auto* vbuf = new VertexBuffer();

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {
                    int blockID = chunk->getBlock(rx, ry, rz);

                    if (blockID)
                    {
                        Blocks::REGISTRY[blockID]->getVertexData(vbuf, world, chunk->position, glm::vec3(rx, ry, rz));
                    }
                }
            }
        }

        if (vbuf->vertexCount() == 0)
            return nullptr;  // skip empty chunk.
        return vbuf;
    }

    static void putCube(VertexBuffer *vbuf, glm::vec3 rpos, Chunk *chunk, World *world, AtlasFrag* frag) {
        for (int i = 0; i < 6; ++i) {
            glm::vec3 dir = dirCubeFace(i);
            auto neib = world->getBlock(chunk->position + rpos + dir);

            if (//Chunk::outbound(adjacent) ||
                //chunk->getBlock(adjacent) == 0
                neib == 0) {
                putFace(vbuf, i, rpos, frag);
            }
        }
    }

    static void putFace(VertexBuffer* vbuf, int face, glm::vec3 rpos, AtlasFrag* frag) {
        // put pos
        for (int i = 0; i < 6; ++i) {  // 6 pos vecs.
            int bas = face*18+i*3;  // 18 = 6vec * 3scalar
            vbuf->addpos(CUBE_POS[bas]+rpos.x, CUBE_POS[bas+1]+rpos.y, CUBE_POS[bas+2]+rpos.z);
            vbuf->addnorm(CUBE_NORM[bas], CUBE_NORM[bas+1], CUBE_NORM[bas+2]);
        }
        // put uv
        for (int i = 0; i < 6; ++i) {
            int bas = face*12 + i*2 ;  // 12=6vec*2f
            vbuf->adduv(CUBE_UV[bas]   * frag->scale.x + frag->offset.x,
                       CUBE_UV[bas+1] * frag->scale.y + frag->offset.y);
        }
    }

    static glm::vec3 dirCubeFace(int face) {
        int bas = face*18;
        return glm::vec3(CUBE_NORM[bas], CUBE_NORM[bas+1], CUBE_NORM[bas+2]);
    }

};

#endif //ETHERTIA_CHUNKMESHGEN_H
