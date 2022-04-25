//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKMESHGEN_H
#define ETHERTIA_CHUNKMESHGEN_H

#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/init/Blocks.h>
#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/client/Loader.h>

class ChunkMeshGen
{
public:

    // Winding:
    // [2,4] +---+ [1]
    //       | \ |
    //   [5] +---+ [0,3]
    static float CUBE_POS[];
//    = {
//            // Left -X
//            0, 0, 1, 0, 1, 1, 0, 1, 0,
//            0, 0, 1, 0, 1, 0, 0, 0, 0,
//            // Right +X
//            1, 0, 0, 1, 1, 0, 1, 1, 1,
//            1, 0, 0, 1, 1, 1, 1, 0, 1,
//            // Bottom -Y
//            0, 0, 1, 0, 0, 0, 1, 0, 0,
//            0, 0, 1, 1, 0, 0, 1, 0, 1,
//            // Bottom +Y
//            0, 1, 1, 1, 1, 1, 1, 1, 0,
//            0, 1, 1, 1, 1, 0, 0, 1, 0,
//            // Front -Z
//            0, 0, 0, 0, 1, 0, 1, 1, 0,
//            0, 0, 0, 1, 1, 0, 1, 0, 0,
//            // Back +Z
//            1, 0, 1, 1, 1, 1, 0, 1, 1,
//            1, 0, 1, 0, 1, 1, 0, 0, 1,
//    };
    static float CUBE_UV[];
//    = {
//            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,  // One Face.
//            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
//            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
//            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
//            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
//            1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
//    };


    static void genMesh(Chunk* chunk) {
        VertexBuffer vbuf;

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {

                    int blockId = chunk->getBlock(rx, ry, rz);

                    if (blockId == Blocks::STONE) {
                        putCube(vbuf, glm::vec3(rx, ry, rz));
                    }

                }
            }
        }


        chunk->model = Loader::loadModel(&vbuf);
    }

    static void putCube(VertexBuffer& vbuf, glm::vec3 rpos) {
        for (int i = 0; i < 6; ++i) {
            putFace(vbuf, i, rpos);
        }
    }

    static void putFace(VertexBuffer& vbuf, int face, glm::vec3 rpos) {
        // put pos
        for (int i = 0; i < 6; ++i) {  // 6 pos vecs.
            int bas = face*18+i*3;  // 18 = 6vec * 3scalar
            vbuf.addpos(CUBE_POS[bas]+rpos.x, CUBE_POS[bas+1]+rpos.y, CUBE_POS[bas+2]+rpos.z);
        }
        // put uv
        for (int i = 0; i < 6; ++i) {
            int bas = face*12 + i*2 ;  // 12=6vec*2f
            vbuf.adduv(CUBE_UV[bas], CUBE_UV[bas+1]);
        }
    }


};

#endif //ETHERTIA_CHUNKMESHGEN_H
