//
// Created by Dreamtowards on 2022/5/5.
//


#include <ethertia/client/render/chunk/ChunkMeshGen.h>
#include "Block.h"

void Block::internalPutCube(VertexBuffer *vbuf, glm::vec3 rpos, Chunk* chunk, TextureAtlas::AtlasFragment* frag) {

    ChunkMeshGen::putCube(vbuf, rpos, chunk, frag);
}

void Block::internalPutLeaves(VertexBuffer *vbuf, glm::vec3 rpos, Chunk* chunk, TextureAtlas::AtlasFragment* frag) {

    float deg45 = Mth::PI / 4.0f;

    float s = 1.5f;

    ChunkMeshGen::putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
                                                    Mth::matEulerAngles(glm::vec3(0.0f, deg45, 0.0f)),
                                                    glm::vec3(1.5f, 1.0f, 1.5f)*s), frag);
    ChunkMeshGen::putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
                                                   Mth::matEulerAngles(glm::vec3(0.0f, -deg45, 0.0f)),
                                                   glm::vec3(1.5f, 1.0f, 1.5f)*s), frag);

    ChunkMeshGen::putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
                                                   Mth::matEulerAngles(glm::vec3(0, Mth::PI_2, deg45)),
                                                   glm::vec3(1.0f, 1.5f, 1.0f)*s), frag);
    ChunkMeshGen::putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
                                                   Mth::matEulerAngles(glm::vec3(0, Mth::PI_2, -deg45)),
                                                   glm::vec3(1.0f, 1.5f, 1.0f)*s), frag);
}
