//
// Created by Dreamtowards on 2022/10/13.
//

#ifndef ETHERTIA_BLOCKYMESHGEN_H
#define ETHERTIA_BLOCKYMESHGEN_H

#include <glm/vec3.hpp>

#include <ethertia/init/MaterialTextures.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/init/MaterialMeshes.h>

class BlockyMeshGen
{
public:

    static void gen(Chunk* chunk, VertexData* vtx, bool genVegetable)
    {
        using glm::vec3;

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {
                    vec3 rp(rx, ry, rz);
                    Cell& c = chunk->getCell(rx, ry, rz);

                    if (!c.mtl)
                        continue;

                    const Material* mtl = c.mtl;
                    int texId = Material::REGISTRY.getOrderId(mtl);

                    if (genVegetable)   // Generating Vegetable
                    {
                        if (!mtl->m_IsVegetable)
                            continue;

                        else if (mtl == Materials::WATER)
                        {
                            putCube(vtx, rp, chunk, mtl);
                        }
                        else if (mtl->m_CustomMesh)
                        {
                            // 如果本材料是 '触地' 的，那么获取底部Cell 并将本材料模型 往下移动 e.g. Tall grass. Crops.
                            glm::vec3 gp = rp;  // gen pos.
                            // 太多bug了 先不用
//                            if (mtl->m_IsTouchdown) {
//                                Cell& down = World::_GetCell(chunk, rp + vec3(0, -1, 0));
//                                if (down.isSmoothTerrain()) {
//                                    gp += down.fp + vec3(0, -1, 0);
//                                }
//                            }

                            putOBJ(vtx, gp, *chunk, texId, *(VertexData*)mtl->m_VertexBuffer);
                        }
                    }
                    else
                    {
                        if (mtl->m_IsVegetable)
                            continue;

                        if (mtl->m_CustomMesh)
                        {
                            putOBJ(vtx, rp, *chunk, texId, *(VertexData*)mtl->m_VertexBuffer);
                        }
                        else if (c.exp_meta == 1)
                        {
                            putCube(vtx, rp, chunk, mtl);
                        }
                    }

                }
            }
        }

    }

    static void putOBJ(VertexData* vtx, glm::vec3 rp, Chunk& chunk, int mtlId, const VertexData& obj_vtx)
    {
        glm::vec2 uvMul = MaterialTextures::uv_mul();
        glm::vec2 uvAdd = MaterialTextures::uv_add(mtlId);
        uvAdd.x += mtlId;  // set mtl id.

        rp += glm::vec3(0.5, 0.0, 0.5);  // offset to cell horiz center.

        vtx->m_Vertices.reserve(vtx->m_Vertices.size() + obj_vtx.m_Vertices.size());

        for (auto& vert : obj_vtx)
        {
            vtx->addVert({vert.pos + rp,
                          vert.tex * uvMul + uvAdd,
                          vert.norm});
        }
    }


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


    static void putCube(VertexData* vtx, glm::vec3 rpos, Chunk* chunk, const Material* mtl) {

        for (int i = 0; i < 6; ++i) {
            glm::vec3 dir = _CubeFaceDir(i);
            glm::vec3 np = rpos + dir;
            Cell& neib = World::_GetCell(chunk, np);

            if (!neib.isOpaqueCube() && !(mtl == Materials::WATER && mtl==neib.mtl))  // and not same mtl e.g. water
            {
                putCubeFace(vtx, i, rpos, mtl->m_TexId);
            }
        }
    }

    static void putCubeFace(VertexData* vtx, int face, glm::vec3 rpos, int mtlId) {
        for (int i = 0; i < 6; ++i)  // 6 verts
        {
            int bas = face*18 + i*3;  // 18 = 6vec * 3scalar
            glm::vec3 pos (CUBE_POS[bas]+rpos.x, CUBE_POS[bas+1]+rpos.y, CUBE_POS[bas+2]+rpos.z);
            glm::vec3 norm(CUBE_NORM[bas], CUBE_NORM[bas+1], CUBE_NORM[bas+2]);

            //bas = face*12 + i*2 ;  // 12=6vec*2f
            //glm::vec2 uv(CUBE_UV[bas], CUBE_UV[bas+1]);
            glm::vec2 uv{mtlId, 1000};

            vtx->addVert({pos, uv, norm});
        }
    }

    static glm::vec3 _CubeFaceDir(int face) {
        int bas = face*18;
        return glm::vec3(CUBE_NORM[bas], CUBE_NORM[bas+1], CUBE_NORM[bas+2]);
    }

//    static void putFace(VertexBuffer* vbuf, glm::vec3 rpos, glm::mat4 trans, int mtlId) {
//        using glm::vec3;
//        using glm::vec2;
//        for (int i = 0; i < 6; ++i) {  // 6 vertices
//            // Pos
//            float* _p = &CUBE_POS_CENT[i*3];  //CUBE_POS_CENT
//            vec3 p = vec3(_p[0], _p[1], _p[2]);
//            p = trans * glm::vec4(p, 1.0f);
//            vbuf->addpos(rpos.x + p.x, rpos.y + p.y, rpos.z + p.z);
//
//            // Norm
//            float* _n = &CUBE_NORM[i*3];
//            vec3 n = vec3(_n[0], _n[1], _n[2]);
//            n = trans * glm::vec4(n, 0.0f);
//            n = glm::normalize(n);
//            vbuf->addnorm(n.x, n.y, n.z);
//
//            // UV
//            float* _u = &CUBE_UV[i*2];
//            vec2 uv = vec2(_u[0], _u[1]);
//            uv = MaterialTextures::uv_to_atlas_region(uv, mtlId);
//            vbuf->adduv(uv.x, uv.y);
//            vbuf->set_uv_mtl(mtlId);
//        }
//    }


//    static void putLeaves(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk, int mtlId) {
//
//        float deg45 = Mth::PI / 4.0f;
//
//        float s = 1.5f;
//
//        putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
//                                         Mth::matEulerAngles(glm::vec3(0.0f, deg45, 0.0f)),
//                                         glm::vec3(1.5f, 1.0f, 1.5f)*s), mtlId);
//        putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
//                                         Mth::matEulerAngles(glm::vec3(0.0f, -deg45, 0.0f)),
//                                         glm::vec3(1.5f, 1.0f, 1.5f)*s), mtlId);
//        putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
//                                         Mth::matEulerAngles(glm::vec3(0, Mth::PI_2, deg45)),
//                                         glm::vec3(1.0f, 1.5f, 1.0f)*s), mtlId);
//        putFace(vbuf, rpos,Mth::matModel(glm::vec3(0.5f, 0.5f, 0.5f),
//                                                       Mth::matEulerAngles(glm::vec3(0, Mth::PI_2, -deg45)),
//                                                       glm::vec3(1.0f, 1.5f, 1.0f)*s), mtlId);
//    }
//
//
//
//
//    static void putTallgrassStarMesh(VertexBuffer* vbuf, glm::vec3 pos, int mtlId) {
//
//        float s = 1.2f;
//
//        glm::vec3 off(0, 0.7, 0);
//
//        float nv3 = Mth::PI / 3.0f;
//        putFace(vbuf, pos, Mth::matModel(off,
//                                         Mth::matEulerAngles(glm::vec3(0.0f, 0, 0.0f)),
//                                         glm::vec3(1)*s), mtlId);
//        putFace(vbuf, pos, Mth::matModel(off,
//                                         Mth::matEulerAngles(glm::vec3(0.0f, nv3, 0.0f)),
//                                         glm::vec3(1)*s), mtlId);
//        putFace(vbuf, pos, Mth::matModel(off,
//                                         Mth::matEulerAngles(glm::vec3(0.0f, nv3*2, 0.0f)),
//                                         glm::vec3(1)*s), mtlId);
//
//    }

//    static void genGrasses(VertexBuffer* vbuf, const std::vector<glm::vec3>& grass_fp) {
//
//        for (int i = 0; i < grass_fp.size(); ++i) {
//            glm::vec3 p = grass_fp[i];
//
//            putTallgrassStarMesh(vbuf, p, Materials::TALLGRASS->m_TexId);
//        }
//
//    }
};

#endif //ETHERTIA_BLOCKYMESHGEN_H
