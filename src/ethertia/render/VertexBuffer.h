//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_VERTEXBUFFER_H
#define ETHERTIA_VERTEXBUFFER_H

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <ethertia/util/Mth.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>
#include <ethertia/util/VertexProcess.h>
#include <ethertia/material/Material.h>

// 3d positions.
class VertexBuffer
{
public:
    std::vector<float> positions;
    std::vector<float> textureCoords;
    std::vector<float> normals;
//    std::vector<float> materialIds;

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

    // CNS 只是额外地添加了MtlId到uv中，为了shaders去判断 可以做特殊处理
    // just set additional MtlId info into UV.x (uv=(u+MtlId, v)), for shaders spec process.
    // please Dont lookup MtlId in these high-density-call place.
    void set_uv_mtl(int MtlId) {
        assert(textureCoords.size()/2 == positions.size()/3);
        textureCoords[textureCoords.size()-2] += MtlId;
    }

    /// CNS 表示这个顶点是"纯MTL"，即 他的uv是无效的 会自动生成"UV"(triplanar mapping)，采集其MtlId的tex
    void add_pure_mtl(int MtlId) {
        adduv(MtlId, 1000);
        assert(textureCoords.size()/2 == positions.size()/3);
    }

    [[nodiscard]] size_t vertexCount() const {
        return positions.size() / 3;
    }

    void initnorm(bool smooth = false) {

        normals.reserve(positions.size());
//        VertexProcess::othonorm(vertexCount(), positions.data(), normals.data(), smooth);

//        if (smooth) {
//            avgnorm();
//        }

        VertexProcess::gen_avgnorm(vertexCount(), positions.data(), vertexCount(), normals.data());

    }

    void avgnorm() {
        VertexProcess::avgnorm(vertexCount(), positions.data(), normals.data());
    }

};

#endif //ETHERTIA_VERTEXBUFFER_H
