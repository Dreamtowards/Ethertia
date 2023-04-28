//
// Created by Dreamtowards on 2023/3/30.
//

#ifndef ETHERTIA_VERTEXDATA_H
#define ETHERTIA_VERTEXDATA_H


#include <vector>


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
    };

    VertexData(const std::string& _filename);
    ~VertexData();

    const void* data() const;
    size_t size() const;

    size_t vertexCount() const;
    bool isIndexed() const;

    // make an Unique Indexed VertexData. hash-map method.
    static VertexData* makeIndexed(const VertexData* nonIndexed);

    std::vector<Vertex> m_Vertices;
    std::vector<int> m_Indices;  // if empty, means vertices are not indexed.



    std::string m_Filename;  // dbg
};


#endif //ETHERTIA_VERTEXDATA_H
