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



class VertexBuffer
{
public:
    using vec3 = glm::vec3;

    // required 3d positions.
    std::vector<float> positions;
    std::vector<float> textureCoords;
    std::vector<float> normals;
//    std::vector<float> materialIds;

    void addpos(float x, float y, float z) {
        positions.push_back(x);
        positions.push_back(y);
        positions.push_back(z);
    }
    void addpos(const vec3& p) {
        addpos(p.x, p.y, p.z);
    }
    vec3 atpos(int vi) const {
        int i = vi*3;
        return {positions[i], positions[i+1], positions[i+2]};
    }

    void adduv(float x, float y) {
        textureCoords.push_back(x);
        textureCoords.push_back(y);
    }
    void adduv(const glm::vec2& uv) {
        adduv(uv.x, uv.y);
    }
    glm::vec2 atuv(int vi) const {
        int i = vi*2;
        return {textureCoords[i], textureCoords[i+1]};
    }


    void addnorm(float x, float y, float z) {
        normals.push_back(x);
        normals.push_back(y);
        normals.push_back(z);
    }
    void addnorm(const vec3& n) {
        addnorm(n.x, n.y, n.z);
    }
    vec3 atnorm(int vi) const {
        int i = vi*3;
        return {normals[i], normals[i+1], normals[i+2]};
    }


    // CNS 只是额外地添加了MtlId到uv中，为了shaders去判断 可以做特殊处理
    // just set additional MtlId info into UV.x (uv=(u+MtlId, v)), for shaders spec process.
    // please Dont lookup MtlId in these high-density-call place.
    void set_uv_mtl(int MtlId) {
        assert(textureCoords.size()/2 == positions.size()/3);
        textureCoords[textureCoords.size()-2] += MtlId;
    }

    /// CNS 表示这个顶点是"纯MTL"，即 他的uv是无效的 会自动生成"UV"(triplanar mapping)，采集其MtlId的tex
    void add_uv_mtl_pure(int MtlId) {
        adduv(MtlId, 1000);
        assert(textureCoords.size()/2 == positions.size()/3);
    }

    [[nodiscard]] size_t vertexCount() const {
        return positions.size() / 3;
    }

    void reserve(size_t verts) {
        positions.reserve(verts*3);
        textureCoords.reserve(verts*2);
        normals.reserve(verts*3);
    }


    void add_vbuf(const VertexBuffer& vbuf, const vec3& rpos, int MtlId, glm::vec2 uvAdd = {0,0}, glm::vec2 uvMul = {1,1}) {

        positions.reserve(positions.size() + vbuf.positions.size());
        normals.reserve(normals.size() + vbuf.normals.size());
        textureCoords.reserve(textureCoords.size() + vbuf.textureCoords.size());

        // CNS. 有很多优化的空间
        for (int i = 0; i < vbuf.vertexCount(); ++i) {
            addpos(vbuf.atpos(i) + rpos);
            addnorm(vbuf.atnorm(i));

            adduv(vbuf.atuv(i)*uvMul + uvAdd);
            set_uv_mtl(MtlId);
        }

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
