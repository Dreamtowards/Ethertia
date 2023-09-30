

#include "MeshGen.h"

#include <stdx/stdx.h>
#include <ethertia/world/Chunk.h>

#include <ethertia/material/Material.h>

#include <ethertia/util/Log.h>


#pragma region Blocky Cube


static float CUBE_POS[] = {
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
static float CUBE_UV[] = {
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,  // One Face.
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
};
static float CUBE_NORM[] = {
        -1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
        0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1,
        0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1
};

static void _PutCube(VertexData& vtx, glm::ivec3 lp, Chunk& chunk, Cell& cell)
{
    for (int face_i = 0; face_i < 6; ++face_i)
    {
        glm::ivec3 face_dir = stdx::cast<glm::vec3>(&CUBE_NORM[face_i * 18]);   // 18: 3 scalar * 3 vertex * 2 triangle

        Cell neibCell = chunk.GetCell(lp + face_dir, true);
        if (!neibCell.IsSolid())  // !IsOpaque()
        {
            for (int vert_i = 0; vert_i < 6; ++vert_i)
            {
                vtx.AddVertex({
                    stdx::cast<glm::vec3>(&CUBE_POS[face_i * 18 + vert_i * 3]) + glm::vec3(lp),
                    {0, 0},  // stdx::cast<glm::vec2>(&CUBE_UV[face_i * 12 + vert_i * 2]),
                    stdx::cast<glm::vec3>(&CUBE_NORM[face_i * 18 + vert_i * 3])
                    });
            }
        }
    }
}



#pragma endregion




#pragma region MeshGen_SN : SurfaceNets

class MeshGen_SN
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


    static bool SN_SignChanged(const Cell& c0, const Cell& c1) {
        return c0.value > 0 != c1.value > 0;
    }

    // Evaluate Normal of a Cell FeaturePoint
    // via Approxiate Differental Gradient  
    // DEL: WARN: may produce NaN Normal Value if the Cell's value is NaN (Nil Cell in the Context)
    static glm::vec3 SN_Grad(glm::vec3 lp, Chunk& chunk)
    {
        const float E = 1.0f;  // Epsilon
        float denom = 1.0f / (2.0f * E);
        using glm::vec3;
        return glm::normalize(vec3(  // ?Needs: L - Mid - R
            chunk.GetCell(lp + vec3(E, 0, 0), true).value - chunk.GetCell(lp - vec3(E, 0, 0), true).value,
            chunk.GetCell(lp + vec3(0, E, 0), true).value - chunk.GetCell(lp - vec3(0, E, 0), true).value,
            chunk.GetCell(lp + vec3(0, 0, E), true).value - chunk.GetCell(lp - vec3(0, 0, E), true).value
        ) * denom);
    }

    // Naive SurfaceNets Method of Evaluate FeaturePoint.
    // return in-cell point.
    static vec3 SN_FeaturePoint(vec3 lp, Chunk& chunk)
    {
        int signchanges = 0;  // numIntersects
        vec3 fp_sum = vec3(0);

        for (int edge_i = 0; edge_i < 12; ++edge_i) {
            const int* edge = EDGE[edge_i];
            vec3 v0 = VERT[edge[0]];
            vec3 v1 = VERT[edge[1]];
            const Cell& c0 = chunk.GetCell(lp + v0, true);
            const Cell& c1 = chunk.GetCell(lp + v1, true);

            if (SN_SignChanged(c0, c1)) {  // sign-change, surface intersection.
                float t = Math::InverseLerp(0.0f, c0.value, c1.value);
                
                //ET_ASSERT(std::isfinite(t));
                if (!std::isfinite(t)) t = 0;  // t maybe -Inf if accessing a Nil Cell.

                vec3 p = t * (v1 - v0) + v0;  // (v1-v0) must > 0. since every edge vert are min-to-max

                fp_sum += p;
                ++signchanges;
            }
        }

        ET_ASSERT(signchanges != 0 &&"FpEval Error: No SignChange.");
        ET_ASSERT(Math::IsFinite(fp_sum) &&"FpEval Error: Non-Finite Fp Value.");

        //if (numIntersects == 0) return { 0,0,0 };  // Dont return NaN.
        //        assert(p.x >= 0.0f && p.x <= 1.0f &&
        //               p.y >= 0.0f && p.y <= 1.0f &&
        //               p.z >= 0.0f && p.z <= 1.0f);
        return fp_sum / (float)signchanges;
    }





    static void SN_Contouring(Chunk& chunk, VertexData& vtx) {

        for (int dx = 0; dx < 16; ++dx) 
        {
            for (int dy = 0; dy < 16; ++dy) 
            {
                for (int dz = 0; dz < 16; ++dz) 
                {
                    vec3 lp(dx, dy, dz);
                    Cell c0 = chunk.GetCell(lp);

                    // for 3 axes edges, if sign-changed, connect adjacent 4 cells' vertices
                    for (int axis_i = 0; axis_i < 3; ++axis_i) 
                    {
                        Cell c1 = chunk.GetCell(lp + AXES[axis_i], true);

                        // Skip the face constructing with a Nil Cell. since the FeaturePoint and Normal(SDF Grad) cannot be evaluated.
                        // ?but Outside should be Enclosed? or the Shadow Casting will be a problem. if a Ceil in TopNil
                        //if (c1.IsNil())
                        //    continue;

                        if (!SN_SignChanged(c0, c1))
                            continue;

                        bool ccw = c0.IsSolid();  // is positive normal. if c0 is solid.
                        Cell& solid = c0.IsSolid() ? c0 : c1;

                        // connect the quad.
                        for (int quadv_i = 0; quadv_i < 6; ++quadv_i) {
                            int wind_vi = ccw ? quadv_i : 5 - quadv_i;

                            vec3 quadp = lp + ADJACENT[axis_i][wind_vi];
                            Cell c = chunk.GetCell(quadp, true);

                            vec3& fp = c.fp;
                            if (!c.IsFpValid())   // only eval when cache invalid.
                            {
                                fp = SN_FeaturePoint(quadp, chunk);
                                c.norm = -SN_Grad(quadp, chunk);

                                if (!Math::IsFinite(c.norm))
                                    c.norm = { 1, 0, 0 };
                            }

                            vec3 pos = quadp + fp;


                            // determine the MtlId of 8 corners. use Nearest Positive Density Corner.
                            float min_dist = Math::Inf;
                            Material* mtl = 0;
                            for (vec3 cellv : VERT) {
                                const Cell& c = chunk.GetCell(quadp + cellv, true);
                                if (c.mtl && c.value > 0 && c.value < min_dist) {
                                    min_dist = c.value;
                                    mtl = c.mtl;
                                }
                            }

                            //ET_ASSERT(mtl != 0);
                            if (mtl == 0) Log::warn("MtlId == 0 Cell.");

                            int MtlId = mtl ? mtl->m_TexId : -1;
                            static const int PURE_MTL_MAGICNUMBER = -1;

                            vtx.AddVertex({ 
                                pos, 
                                glm::vec2(MtlId, PURE_MTL_MAGICNUMBER),
                                c.norm 
                                });
                        }
                        
                    }
                }
            }
        }
    }


};


#pragma endregion



void MeshGen::GenerateMesh(Chunk& chunk, VertexData& vtx)
{

    MeshGen_SN::SN_Contouring(chunk, vtx);
    return;

    for (int dx = 0; dx < 16; ++dx)
    {
        for (int dy = 0; dy < 16; ++dy)
        {
            for (int dz = 0; dz < 16; ++dz)
            {
                glm::ivec3 lp{dx, dy, dz};
                Cell& cell = chunk.LocalCell(lp);

                if (cell.IsSolid())
                {
                    _PutCube(vtx, lp, chunk, cell);
                }
            }
        }
    }

}