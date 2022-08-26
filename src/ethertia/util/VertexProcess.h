//
// Created by Dreamtowards on 2022/8/26.
//

#ifndef ETHERTIA_VERTEXPROCESS_H
#define ETHERTIA_VERTEXPROCESS_H

class VertexProcess
{
public:



    static void avgnormN2(size_t vertCount, const float* pos, float* norm) {
        using glm::vec3;

        for (int i = 0; i < vertCount; ++i) {
            // for each vertex, find shared-position vertex, avg their norm
            vec3 p = Mth::vec3(&pos[i*3]);

            vec3 n = Mth::vec3(&norm[i*3]);
            for (int j = 0; j < vertCount; ++j) {
                if (i == j) continue;
                vec3 _p = Mth::vec3(&pos[j*3]);
                vec3 _n = Mth::vec3(&norm[j*3]);

                if (p == _p && glm::dot(glm::normalize(n), glm::normalize(_n)) > 0.7f) {
                    n += _n;
                }
            }

            n = glm::normalize(n);  // n /= numShared is not suit, because N may not normalized vector, may is weighted.

            if (std::isnan(glm::length(n))) {
                Log::info("N2 Nan");
            }

            norm[i*3]   = n.x;
            norm[i*3+1] = n.y;
            norm[i*3+2] = n.z;
        }
    }
};

#endif //ETHERTIA_VERTEXPROCESS_H
