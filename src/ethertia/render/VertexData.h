//
// Created by Dreamtowards on 2023/3/30.
//

#ifndef ETHERTIA_VERTEXDATA_H
#define ETHERTIA_VERTEXDATA_H


#include <vector>
#include <string>

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
        Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm);

        bool operator==(const Vertex& o) const;

        static size_t stride();
    };

    VertexData();
    ~VertexData();

    const void* data() const;  // vtx_data
    size_t size() const;       // vtx_size

    const void* vtx_data() const;
    size_t vtx_size() const;

    const void* idx_data() const;
    size_t idx_size() const;


    bool isIndexed() const;

    // all vertices, not just unique vertices number.
    size_t vertexCount() const;

    // access vertex. by index if isIndexed(). useful for iteration of {vert(i) : vertexCount()}
    const Vertex& vert(int i) const;

    void addVert(const Vertex& vert);

    // make an Unique Indexed VertexData. hash-map method.
    static VertexData* makeIndexed(const VertexData* nonIndexed);

    std::vector<Vertex> m_Vertices;
    std::vector<int> m_Indices;  // if empty, means vertices are not indexed.


    struct VertIter
    {
        size_t m_idx;
        const VertexData* m_vtx;

        const Vertex& operator*() const { return m_vtx->vert(m_idx); }
        VertIter operator++() { ++m_idx; return *this; }
        VertIter operator--() { --m_idx; return *this; }
        friend bool operator!=(const VertIter& a, const VertIter& b) { return a.m_idx!=b.m_idx; }

    };
    VertIter begin() const {
        return VertIter{0, this};
    }
    VertIter end() const {
        return VertIter{vertexCount(), this};
    }


    std::string m_Filename;  // dbg
};


// hash func define. for std::unordered_map.
namespace std {
    template<> struct hash<VertexData::Vertex> {
        size_t operator()(VertexData::Vertex const& vertex) const;
    };
}


#endif //ETHERTIA_VERTEXDATA_H
