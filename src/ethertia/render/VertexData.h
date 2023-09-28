//
// Created by Dreamtowards on 2023/3/30.
//

#pragma once

#include <vector>
#include <string>
#include <span>

#include <glm/glm.hpp>


// an "interleaved vertex data" structure. (more compact, higher memory access efficiency. compare to 'separate vertex data' struct)
// unlike VertexBuffer which just a handle/pointer to a block of memory in GPU. (it's data in GPU).

class VertexData
{
public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 norm;

        Vertex() = default;
        Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm) : pos(pos), tex(tex), norm(norm) {}

        bool operator==(const Vertex& o) const { return pos == o.pos && tex == o.tex && norm == o.norm; }

        static size_t stride()
        {
            static_assert(sizeof(VertexData::Vertex) == 32);  // sizeof(float) * 8, {vec3 pos, vec2 uv, vec3 norm}
            return sizeof(VertexData::Vertex);
        }
    };

    VertexData() = default;
    ~VertexData();

    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;  // if empty, means vertices are not indexed.

    std::string m_Filename;  // dbg



    const void* vtx_data() const { return Vertices.data(); }
    size_t      vtx_size() const { return sizeof(Vertices[0]) * Vertices.size(); }

    const void* idx_data() const { return Indices.data(); }
    size_t      idx_size() const { return sizeof(Indices[0]) * Indices.size(); }

    std::span<const char> vtx_span() const { return { (const char*)vtx_data(), vtx_size() }; }
    std::span<const char> idx_span() const { return { (const char*)idx_data(), idx_size() }; }

    bool IsIndexed() const { return !Indices.empty(); }

    [[nodiscard]] const Vertex& at(uint32_t i) const { return IsIndexed() ? Vertices[Indices[i]] : Vertices[i]; }

    uint32_t VertexCount() const { return IsIndexed() ? Indices.size() : Vertices.size(); }


    // Fast Add / Remove. if IsIndexed, the compression will decline.

    void AddVertex(const Vertex& vert)
    {
        if (IsIndexed())
        {
            Indices.push_back(Vertices.size());
        }
        Vertices.emplace_back(vert);
    }

    void RemoveVertex(int index, int count = 1)
    {
        if (IsIndexed())
        {
            auto it = Indices.begin() + index;
            Indices.erase(it, it + count);
        }
        else
        {
            auto it = Vertices.begin() + index;
            Vertices.erase(it, it + count);
        }
    }


    // make an Unique Indexed VertexData. hash-map method.
    static VertexData* MakeIndexed(const VertexData* nonIndexed, VertexData* out = new VertexData());


    struct VertIter
    {
        uint32_t m_idx;
        const VertexData* m_vtx;

        const Vertex& operator*() const { return m_vtx->at(m_idx); }
        VertIter operator++() { ++m_idx; return *this; }
        VertIter operator--() { --m_idx; return *this; }
        friend bool operator!=(const VertIter& a, const VertIter& b) { return a.m_idx!=b.m_idx; }

    };
    VertIter begin() const {
        return VertIter{0, this};
    }
    VertIter end() const {
        return VertIter{VertexCount(), this};
    }


};


// hash func define. for std::unordered_map.
namespace std {
    template<> struct hash<VertexData::Vertex> {
        size_t operator()(VertexData::Vertex const& vertex) const;
    };
}

