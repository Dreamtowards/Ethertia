//
// Created by Dreamtowards on 2022/8/23.
//

#ifndef ETHERTIA_SURFACENETSMESHGEN_H
#define ETHERTIA_SURFACENETSMESHGEN_H


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


    static VertexBuffer* contouring(Chunk* chunk) {
        VertexBuffer* vbuf = new VertexBuffer();

        // range [1, 15], boundary-exclusive
        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {
                    vec3 rp(rx, ry, rz);
                    Cell& c0 = World::_GetCell(chunk, rp);

                    // for 3 axes edges, if sign-changed, connect adjacent 4 cells' vertices
                    for (int axis_i = 0; axis_i < 3; ++axis_i) {
                        Cell& c1 = World::_GetCell(chunk, rp + AXES[axis_i]);

                        if (c0.density > 0 != c1.density > 0) {  // sign changed.
                            bool ccw = c0.density > 0;  // is positive normal. if c0 is solid.
                            Cell& solid = c0.density > 0 ? c0 : c1;

                            // connect the quad.
                            for (int quadv_i = 0; quadv_i < 6; ++quadv_i) {
                                int wind_vi = ccw ? quadv_i : 5 - quadv_i;

                                vec3 quadp = rp + ADJACENT[axis_i][wind_vi];
                                // Cell& vc = World::_GetBlock(chunk, quadp);
                                vec3 fp = featurepoint(quadp, chunk);

                                vbuf->addpos(quadp + fp);
                                // assert(vc.id != 0);
                                // assert(vc.density > 0);  // density could <= 0, why
                                // vbuf->_add_mtl_id(vc.id);

                                // Redundant Execution, todo Move Out
                                // 1. get material by minor distance (abs(density)).
                                // 2. Optical option, use MostDuplicated Material of the 8 verts.
                                float min_dist = Mth::Inf;
                                int MtlId = -1;
                                for (vec3 cellv : VERT) {
                                    Cell& c = World::_GetCell(chunk, quadp + cellv);
                                    if (c.id && (c.density) < min_dist) {
                                        // assert(c.density > 0);
                                        min_dist = (c.density);
                                        MtlId = c.id;
                                    }
                                }
                                vbuf->_add_mtl_id(MtlId);

                            }
                        }
                    }
                }
            }
        }



        return vbuf;
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

            if (c0.density > 0 != c1.density > 0) {  // sign-change, surface intersection.
                vec3 p = Mth::rlerp(0, c0.density, c1.density) * (v1-v0) + v0;  // (v1-v0) must > 0. since every edge vert are min-to-max

                sumFp += p;
                numIntersects++;
            }
        }

        assert(numIntersects > 0);
        return sumFp / (float)numIntersects;
    }

};

#endif //ETHERTIA_SURFACENETSMESHGEN_H
