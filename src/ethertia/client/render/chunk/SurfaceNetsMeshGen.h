//
// Created by Dreamtowards on 2022/8/23.
//

#ifndef ETHERTIA_SURFACENETSMESHGEN_H
#define ETHERTIA_SURFACENETSMESHGEN_H


#include <ethertia/world/Octree.h>

class SurfaceNetsMeshGen
{
public:

    inline static const glm::vec3 VERT[8] = {
            {0, 0, 0},
            {0, 0, 1},
            {0, 1, 0},
            {0, 1, 1},
            {1, 0, 0},
            {1, 0, 1},
            {1, 1, 0},
            {1, 1, 1}
    };
    inline static const glm::vec3 AXES[3] = {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
    };


    static VertexBuffer* contouring(Chunk* chunk) {
        VertexBuffer* vbuf = new VertexBuffer();
        using glm::vec3;

        // for each Edge, Evaluate FeaturePoint, Compose the 4 Cube's Points,

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {
                    vec3 rp(rx, ry, rz);

                    // for 3 axes edges, if sign-changed, connect adjacent 4 cubes' vertices
                    MaterialStat& b0 = World::_GetBlock(chunk, rp);
                    for (int i = 0; i < 3; ++i) {
                        MaterialStat& b1 = World::_GetBlock(chunk, rp + AXES[i]);

                        if (b0.density > 0 != b1.density > 0) {  // sign changed.


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
