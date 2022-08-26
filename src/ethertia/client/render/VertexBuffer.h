//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_VERTEXBUFFER_H
#define ETHERTIA_VERTEXBUFFER_H

#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/hash.hpp>


#include <ethertia/util/Mth.h>
#include <ethertia/util/testing/BenchmarkTimer.h>
#include <ethertia/util/Log.h>

// 3d positions.
class VertexBuffer
{
public:
    std::vector<float> positions;
    std::vector<float> textureCoords;
    std::vector<float> normals;

    void addpos(float x, float y, float z) {
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
    }
    void addpos(const glm::vec3& p) {
        addpos(p.x, p.y, p.z);
    }

    void adduv(float x, float y) {
        textureCoords.push_back(x);
        textureCoords.push_back(y);
    }

    void addnorm(float x, float y, float z) {
        normals.push_back(x);
        normals.push_back(y);
        normals.push_back(z);
    }
    void addnorm(const glm::vec3& n) {
        addnorm(n.x, n.y, n.z);
    }

    size_t vertexCount() const {
        return positions.size() / 3;
    }

    void initnorm() {

        size_t n = vertexCount();

        normals.reserve(n * 3);
        othonorm(n, positions.data(), normals.data(), false);

    }

    void avgnorm() {
        avgnorm(vertexCount(), positions.data(), normals.data());
    }

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
                if (isnan(w0)) w0 = 1;
                if (isnan(w1)) w1 = 1;
                if (isnan(w2)) w2 = 1;
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
};

#endif //ETHERTIA_VERTEXBUFFER_H
