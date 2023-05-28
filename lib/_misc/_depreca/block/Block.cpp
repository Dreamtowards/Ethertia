//
// Created by Dreamtowards on 2022/5/5.
//


#include <ethertia/client/render/chunk/BlockyChunkMeshGen.h>
#include "Block.h"

void Block::internalPutCube(VertexBuffer *vbuf, glm::vec3 rpos, Chunk* chunk, TextureAtlas::Region* frag) {

    BlockyChunkMeshGen::putCube(vbuf, rpos, chunk, frag);
}

void Block::internalPutLeaves(VertexBuffer *vbuf, glm::vec3 rpos, Chunk* chunk, TextureAtlas::Region* frag, float randf) {

    float deg45 = Mth::PI / 4.0f;

    float s = 1.5f;

    u32 hash = glm::length(rpos)+glm::length(chunk->position)+rpos.x+rpos.y+rpos.z+34729;
    glm::vec3 randOffset(Mth::hash(hash*9983429), Mth::hash(hash*93024), Mth::hash(hash*793424));
    randOffset *= randf;
    randOffset -= glm::vec3(randf*0.5f);

    BlockyChunkMeshGen::putFace(vbuf, rpos, Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f) + randOffset,
                                                          Mth::matEulerAngles(glm::vec3(0.0f, deg45, 0.0f)),
                                                    glm::vec3(1.5f, 1.0f, 1.5f)*s), frag);
    BlockyChunkMeshGen::putFace(vbuf, rpos, Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f) + randOffset,
                                                          Mth::matEulerAngles(glm::vec3(0.0f, -deg45, 0.0f)),
                                                   glm::vec3(1.5f, 1.0f, 1.5f)*s), frag);

    BlockyChunkMeshGen::putFace(vbuf, rpos, Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f) + randOffset,
                                                          Mth::matEulerAngles(glm::vec3(0, Mth::PI_2, deg45)),
                                                   glm::vec3(1.0f, 1.5f, 1.0f)*s), frag);
    BlockyChunkMeshGen::putFace(vbuf, rpos, Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f) + randOffset,
                                                          Mth::matEulerAngles(glm::vec3(0, Mth::PI_2, -deg45)),
                                                   glm::vec3(1.0f, 1.5f, 1.0f)*s), frag);
}
