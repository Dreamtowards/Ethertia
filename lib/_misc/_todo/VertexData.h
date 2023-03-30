//
// Created by Dreamtowards on 2023/3/30.
//

#ifndef ETHERTIA_VERTEXDATA_H
#define ETHERTIA_VERTEXDATA_H

// VertexData is a 'container' that hold vertex data in GPU. (data in CPU (Host)).
// unlike VertexBuffer which just a handle/pointer to a block of memory in GPU. (it's data in GPU).

class VertexData
{
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 norm;
    };

    VertexData(const std::string& _filename);
    ~VertexData();

    const void* data() const { return m_Vertices.data(); }
    size_t size() const { return sizeof(m_Vertices[0]) * m_Vertices.size(); }

    size_t vertexCount() const { return m_Vertices.size(); }


    std::vector<Vertex> m_Vertices;
    std::vector<int> m_Indices;  // if empty, means vertices are not indexed.

    std::string m_Filename;  // dbg
};


#endif //ETHERTIA_VERTEXDATA_H
