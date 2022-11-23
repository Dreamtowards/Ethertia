//
// Created by Dreamtowards on 2022/8/23.
//

#ifndef ETHERTIA_SURFACENETSMESHGEN_H
#define ETHERTIA_SURFACENETSMESHGEN_H


#include <ethertia/world/Octree.h>

class SurfaceNetsMeshGen
{
public:

//    inline static const glm::vec3 VERT[8] = {
//            {0, 0, 0},
//            {0, 0, 1},
//            {0, 1, 0},
//            {0, 1, 1},
//            {1, 0, 0},
//            {1, 0, 1},
//            {1, 1, 0},
//            {1, 1, 1}
//    };
    inline static const glm::vec3 AXES[3] = {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
    };
    inline static const glm::vec3 ADJACENT[3][6] = {
            {{0,0,0}, {0,-1,0}, {0,-1,-1}, {0,-1,-1}, {0,0,-1}, {0,0,0}},
            {{0,0,0}, {0,0,-1}, {-1,0,-1}, {-1,0,-1}, {-1,0,0}, {0,0,0}},
            {{0,0,0}, {-1,0,0}, {-1,-1,0}, {-1,-1,0}, {0,-1,0}, {0,0,0}}
    };


    static VertexBuffer* contouring(Chunk* chunk) {
        VertexBuffer* vbuf = new VertexBuffer();
        using glm::vec3;
        using Cell = MaterialStat;

        // range [1, 15], boundary-exclusive
        for (int rx = 1; rx < 15; ++rx) {
            for (int ry = 1; ry < 15; ++ry) {
                for (int rz = 1; rz < 15; ++rz) {
                    vec3 rp(rx, ry, rz);
                    Cell& c0 = World::_GetBlock(chunk, rp);

                    // for 3 axes edges, if sign-changed, connect adjacent 4 cells' vertices
                    for (int axis_i = 0; axis_i < 3; ++axis_i) {
                        Cell& c1 = World::_GetBlock(chunk, rp + AXES[axis_i]);

                        if (c0.density > 0 != c1.density > 0) {  // sign changed.
                            bool ccw = c0.density > 0;  // is positive normal. if c0 is solid.
                            Cell& solid = c0.density > 0 ? c0 : c1;

                            // connect the quad.
                            for (int vert_i = 0; vert_i < 6; ++vert_i) {
                                int directed_vert_i = ccw ? vert_i : 5 - vert_i;

                                vec3 vcp = rp + ADJACENT[axis_i][directed_vert_i];
                                Cell& vc = World::_GetBlock(chunk, vcp);

                                vbuf->addpos(vcp + vec3(0.5f));
                                assert(vc.id != 0);
                                assert(vc.density > 0);
                                vbuf->_add_mtl_id(vc.id);
                            }
                        }
                    }
                }
            }
        }



        return vbuf;
    }

    /// block-rel point
//    static glm::vec3 featurepoint() {
//
//    }

};

#endif //ETHERTIA_SURFACENETSMESHGEN_H
