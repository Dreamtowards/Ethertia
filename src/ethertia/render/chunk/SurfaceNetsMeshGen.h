//
// Created by Dreamtowards on 2022/8/23.
//

#ifndef ETHERTIA_SURFACENETSMESHGEN_H
#define ETHERTIA_SURFACENETSMESHGEN_H

#include <ethertia/world/Chunk.h>

class SurfaceNetsMeshGen
{
public:
    using vec3 = glm::vec3;

    inline static const vec3 VERT[8] = {
            {0, 0, 0},  // 0
            {0, 0, 1},
            {0, 1, 0},  // 2
            {0, 1, 1},
            {1, 0, 0},  // 4
            {1, 0, 1},
            {1, 1, 0},  // 6
            {1, 1, 1}
    };
    // from min to max in each Edge.  axis order x y z.
    // Diagonal Edge in Cell is in-axis-flip-index edge.  i.e. diag of edge[axis*4 +i] is edge[axis*4 +(3-i)]
    /*     +--2--+    +-----+    +-----+
     *    /|    /|   /7    /6  11|    10
     *   +--3--+ |  +-----+ |  +-----+ |
     *   | +--0|-+  5 +---4-+  | +---|-+
     *   |/    |/   |/    |/   |9    |8
     *   +--1--+    +-----+    +-----+
     *   |3  2| winding. for each axis.
     *   |1  0|
     */
    inline static const int EDGE[12][2] = {
            {0,4}, {1,5}, {2,6}, {3,7},  // X
            {5,7}, {1,3}, {4,6}, {0,2},  // Y
            {4,5}, {0,1}, {6,7}, {2,3}   // Z
    };
    inline static const vec3 AXES[3] = {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
    };
    inline static const vec3 ADJACENT[3][6] = {
            {{0,0,0}, {0,-1,0}, {0,-1,-1}, {0,-1,-1}, {0,0,-1}, {0,0,0}},
            {{0,0,0}, {0,0,-1}, {-1,0,-1}, {-1,0,-1}, {-1,0,0}, {0,0,0}},
            {{0,0,0}, {-1,0,0}, {-1,-1,0}, {-1,-1,0}, {0,-1,0}, {0,0,0}}
    };

    static bool sign_changed(const Cell& c0, const Cell& c1) {
        return c0.density > 0 != c1.density > 0;
    }

    static bool _hasnan(vec3 v) {
        return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
    }

    // Gradient
    static glm::vec3 _EvalNormGrad(glm::vec3 p, Chunk* chunk, float c_dens)
    {
        const float E = 1.0f;  // Epsilon
        float denom = 1.0f / (2.0f * E);
        using glm::vec3;
        return glm::normalize(glm::vec3(
                chunk->g_cell(p + vec3(E,0,0)).density -  /*c_dens,*/chunk->g_cell(p - vec3(E,0,0)).density,
                chunk->g_cell(p + vec3(0,E,0)).density -  /*c_dens,*/chunk->g_cell(p - vec3(0,E,0)).density,
                chunk->g_cell(p + vec3(0,0,E)).density -  /*c_dens */chunk->g_cell(p - vec3(0,0,E)).density
        ) * denom);
    }

    static void contouring(Chunk* chunk, VertexData* vtx) {

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {
                    vec3 rp(rx, ry, rz);
                    Cell& c0 = World::_GetCell(chunk, rp);

                    // for 3 axes edges, if sign-changed, connect adjacent 4 cells' vertices
                    for (int axis_i = 0; axis_i < 3; ++axis_i) {
                        Cell& c1 = World::_GetCell(chunk, rp + AXES[axis_i]);

                        if (sign_changed(c0, c1)) {  // sign changed.
                            bool ccw = c0.density > 0;  // is positive normal. if c0 is solid.
                            Cell& solid = c0.density > 0 ? c0 : c1;

                            // connect the quad.
                            for (int quadv_i = 0; quadv_i < 6; ++quadv_i) {
                                int wind_vi = ccw ? quadv_i : 5 - quadv_i;

                                vec3 quadp = rp + ADJACENT[axis_i][wind_vi];

                                Cell& c = World::_GetCell(chunk, quadp);
                                vec3& fp = c.fp;
                                if (fp.x == Mth::Inf) {  // only eval invalid cell.
                                    fp = featurepoint(quadp, chunk);
                                    c.norm = _EvalNormGrad(quadp, chunk, c.density);
                                }

                                assert(!_hasnan(fp));
                                vec3 pos = quadp + fp;


                                // determine the MtlId of 8 corners. use Nearest Positive Density Corner.
                                float min_dist = Mth::Inf;
                                const Material* mtl = 0;
                                for (vec3 cellv : VERT) {
                                    Cell& c = World::_GetCell(chunk, quadp + cellv);
                                    if (c.mtl && c.density > 0 && c.density < min_dist) {
                                        min_dist = c.density;
                                        mtl = c.mtl;
                                    }
                                }
//                                assert(MtlId != 0);
                                if (mtl == 0) Log::warn("MtlId == 0 Cell.");

                                int mtlId = mtl ? mtl->m_TexId : -1;
                                static const int PURE_MTL_MAGICNUMBER = -1;

                                vtx->m_Vertices.emplace_back(pos, glm::vec2(mtlId, PURE_MTL_MAGICNUMBER), -c.norm);
                            }
                        }
                    }
                }
            }
        }
    }

    // Naive SurfaceNets Method of Evaluate FeaturePoint.
    // return in-cell point.
    static vec3 featurepoint(vec3 rp, Chunk* chunk)
    {
        int numIntersects = 0;
        vec3 sumFp = vec3(0);

        for (int edge_i = 0; edge_i < 12; ++edge_i) {
            const int* edge = EDGE[edge_i];
            vec3 v0 = VERT[edge[0]];
            vec3 v1 = VERT[edge[1]];
            Cell& c0 = World::_GetCell(chunk, rp + v0);
            Cell& c1 = World::_GetCell(chunk, rp + v1);

            if (sign_changed(c0, c1)) {  // sign-change, surface intersection.
                vec3 p = Mth::rlerp(0, c0.density, c1.density) * (v1-v0) + v0;  // (v1-v0) must > 0. since every edge vert are min-to-max

                sumFp += p;
                numIntersects++;
            }
        }

        //ASSERT_WARN(numIntersects > 0, "Illegal FeaturePoint");
        if (numIntersects == 0) {
            return {0,0,0};  // Dont return NaN.
        }
        vec3 p = sumFp / (float)numIntersects;
//        assert(numIntersects > 0);
//        assert(p.x >= 0.0f && p.x <= 1.0f &&
//               p.y >= 0.0f && p.y <= 1.0f &&
//               p.z >= 0.0f && p.z <= 1.0f);
        return p;
    }

};

#endif //ETHERTIA_SURFACENETSMESHGEN_H
