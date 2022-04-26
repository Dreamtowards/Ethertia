//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKMESHGEN_H
#define ETHERTIA_CHUNKMESHGEN_H

#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/init/Blocks.h>
#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/client/Loader.h>
#include <ethertia/client/render/TextureAtlas.h>

class ChunkMeshGen
{
public:

    // Winding:
    // [2,4] +---+ [1]
    //       | \ |
    //   [5] +---+ [0,3]
    // Faces: -X, +X, -Y, +Y, -Z, +Z.
    static float CUBE_POS[];
    static float CUBE_UV[];
    static float CUBE_NORM[];


    static void genMesh(Chunk* chunk) {
        VertexBuffer vbuf;

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {

                    int blockId = chunk->getBlock(rx, ry, rz);

                    if (blockId == Blocks::STONE) {
                        putCube(vbuf, glm::vec3(rx, ry, rz), chunk);
                    }

                }
            }
        }

        chunk->model = Loader::loadModel(&vbuf);
    }

    static void putCube(VertexBuffer& vbuf, glm::vec3 rpos, Chunk* chunk);

    static void putFace(VertexBuffer& vbuf, int face, glm::vec3 rpos, TextureAtlas::AtlasFragment* frag) {
        // put pos
        for (int i = 0; i < 6; ++i) {  // 6 pos vecs.
            int bas = face*18+i*3;  // 18 = 6vec * 3scalar
            vbuf.addpos(CUBE_POS[bas]+rpos.x, CUBE_POS[bas+1]+rpos.y, CUBE_POS[bas+2]+rpos.z);
            vbuf.addnorm(CUBE_NORM[bas], CUBE_NORM[bas+1], CUBE_NORM[bas+2]);
        }
        // put uv
        for (int i = 0; i < 6; ++i) {
            int bas = face*12 + i*2 ;  // 12=6vec*2f
            vbuf.adduv(CUBE_UV[bas]   * frag->scale.x + frag->offset.x,
                       CUBE_UV[bas+1] * frag->scale.y + frag->offset.y);
        }
    }


    static glm::vec3 dirCubeFace(int face) {
        int bas = face*18;
        return glm::vec3(CUBE_NORM[bas], CUBE_NORM[bas+1], CUBE_NORM[bas+2]);
    }

};

#endif //ETHERTIA_CHUNKMESHGEN_H
