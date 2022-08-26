//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_BLOCKYCHUNKMESHGEN_H
#define ETHERTIA_BLOCKYCHUNKMESHGEN_H

#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/client/Loader.h>
#include <ethertia/client/render/TextureAtlas.h>
#include <ethertia/world/World.h>
#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/init/Blocks.h>
#include <ethertia/init/BlockTextures.h>

class BlockyChunkMeshGen
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
    inline static float CUBE_POS_CENT[] = {
            0,-0.5, 0.5f,  0, 0.5f, 0.5f,  0, 0.5f,-0.5f,  // Left -X
            0,-0.5, 0.5f,  0, 0.5f,-0.5f,  0,-0.5f,-0.5f
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
                    BlockState& block = chunk->getBlock(rx, ry, rz);

                    if (block.id)
                    {
                        Blocks::REGISTRY[block.id]->getVertexData(vbuf, glm::vec3(rx, ry, rz), chunk);
                    }
                }
            }
        }

        if (vbuf->vertexCount() == 0)
            return nullptr;  // skip empty chunk.
        return vbuf;
    }

    static void putCube(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk, AtlasFrag* frag) {
//        putCubeFace(vbuf, 2, rpos, frag);
//        return;
        u8 blk = chunk->getBlock(rpos).id;
        for (int i = 0; i < 6; ++i) {
            glm::vec3 dir = dirCubeFace(i);
            glm::vec3 np = rpos + dir;
            u8 neib = Chunk::outbound(np) ? 0 : chunk->getBlock(np).id;

//            bool opaq = neib == Blocks::LEAVES || (neib == Blocks::WATER && neib != blk);
//            if (blk == Blocks::WATER && neib == blk)
//                opaq = false;

            if (neib == 0 || !Blocks::REGISTRY[neib]->isOpaque() ) {
                if (neib == Blocks::WATER && neib == blk) {
                    continue;
                }
                putCubeFace(vbuf, i, rpos, frag);
            }
        }
    }

    static void putCubeFace(VertexBuffer* vbuf, int face, glm::vec3 rpos, AtlasFrag* frag) {
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

    static void putFace(VertexBuffer* vbuf, glm::vec3 rpos, glm::mat4 trans, AtlasFrag* frag) {
        using glm::vec3;
        using glm::vec2;
        for (int i = 0; i < 6; ++i) {  // 6 vertices
            // Pos
            float* _p = &CUBE_POS_CENT[i*3];
            vec3 p = vec3(_p[0], _p[1], _p[2]);
            p = trans * glm::vec4(p, 1.0f);
            vbuf->addpos(rpos.x + p.x, rpos.y + p.y, rpos.z + p.z);

            // Norm
            float* _n = &CUBE_NORM[i*3];
            vec3 n = vec3(_n[0], _n[1], _n[2]);
            n = trans * glm::vec4(n, 0.0f);
            n = glm::normalize(n);
            vbuf->addnorm(n.x, n.y, n.z);

            // UV
            float* _u = &CUBE_UV[i*2];
            vec2 u = vec2(_u[0], _u[1]);
            u = u * frag->scale + frag->offset;
            vbuf->adduv(u.x, u.y);
        }

    }

};

#endif //ETHERTIA_BLOCKYCHUNKMESHGEN_H
