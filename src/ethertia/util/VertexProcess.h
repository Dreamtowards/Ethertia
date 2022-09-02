//
// Created by Dreamtowards on 2022/8/26.
//

#ifndef ETHERTIA_VERTEXPROCESS_H
#define ETHERTIA_VERTEXPROCESS_H

class VertexProcess
{
public:

    static void othonorm(size_t vertCount, const float* pos, float* norm, bool weight = false) {
        using glm::vec3;

        for (int i = 0; i < vertCount*3; i += 9) {  // tri vert
            vec3 v0 = Mth::vec3(&pos[i]);
            vec3 v1 = Mth::vec3(&pos[i+3]);
            vec3 v2 = Mth::vec3(&pos[i+6]);

            float w0 = 1.0f,
                    w1 = 1.0f,
                    w2 = 1.0f;
            if (weight) {
                w0 = glm::angle(glm::normalize(v1 - v0), glm::normalize(v2 - v0));
                w1 = glm::angle(glm::normalize(v2 - v1), glm::normalize(v0 - v1));
                w2 = glm::angle(glm::normalize(v0 - v2), glm::normalize(v1 - v2));

//                Log::info("w012 {}, {}, {}", w0, w1, w2);
                if (std::isnan(w0)) w0 = 1;
                if (std::isnan(w1)) w1 = 1;
                if (std::isnan(w2)) w2 = 1;
            }
            // v0: 0 0 0
            // v1: 0 0 1
            // v2: 0 1 1

            vec3 d = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            if (std::isnan(glm::length(d))) {
//                Log::info("badNorm");
                d = glm::vec3(1, 0, 0);
            }

            Mth::vec3out(d * w0, &norm[i]);
            Mth::vec3out(d * w1, &norm[i+3]);
            Mth::vec3out(d * w2, &norm[i+6]);
        }
    }

    static void avgnorm(size_t vertCount, const float* pos, float* norm) {
        using glm::vec3;

        std::unordered_map<vec3, vec3> pos2norm;

        for (int i = 0; i < vertCount; ++i) {
            vec3 p = Mth::vec3(&pos[i*3]);
            vec3 n = Mth::vec3(&norm[i*3]);

            auto it = pos2norm.find(p);
            if (it == pos2norm.end()) {
                pos2norm[p] = n;
            } else { //if (glm::dot(glm::normalize(n), glm::normalize(it->second)) > 0.7f) {  // smooth only those nearly-parallel normals
                it->second += n;
            }
        }

        // Normalize
        for (auto& it : pos2norm) {
            it.second = glm::normalize(it.second);
        }

        for (int i = 0; i < vertCount; ++i) {
            vec3 p = Mth::vec3(&pos[i*3]);
            auto it = pos2norm.find(p);
            Mth::vec3out(it->second, &norm[i*3]);
        }
    }


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
