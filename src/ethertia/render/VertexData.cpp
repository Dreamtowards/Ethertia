//
// Created by Dreamtowards on 2023/4/27.
//



#include "VertexData.h"




const void* VertexData::data() const {
    return m_Vertices.data();
}
size_t VertexData::size() const {
    return sizeof(m_Vertices[0]) * m_Vertices.size();
}

const void* VertexData::idx_data() const {
    return m_Indices.data();
}
size_t VertexData::idx_size() const {
    return sizeof(m_Indices[0]) * m_Indices.size();
}

size_t VertexData::vertexCount() const
{
    return isIndexed() ? m_Indices.size() : m_Vertices.size();
}

bool VertexData::isIndexed() const
{
    return !m_Indices.empty();
}

const VertexData::Vertex& VertexData::vert(int i) const
{
    return isIndexed() ? m_Vertices[m_Indices[i]] : m_Vertices[i];
}


void VertexData::addVert(const Vertex& vert)
{
    if (isIndexed())
    {
        m_Indices.push_back(m_Vertices.size());
    }

    m_Vertices.emplace_back(vert);
}






#include <ethertia/util/Log.h>



VertexData::VertexData() {}

VertexData::~VertexData()
{
    Log::info("Delete VertexData: {} with {} vertices", m_Filename, m_Vertices.size());
}


VertexData::Vertex::Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm) :
        pos(pos), tex(tex), norm(norm) {}







#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

size_t std::hash<VertexData::Vertex>::operator()(const VertexData::Vertex &vertex) const
{
    return ((hash<glm::vec3>()(vertex.pos) ^
            (hash<glm::vec2>()(vertex.tex)  << 1)) >> 1) ^
            (hash<glm::vec3>()(vertex.norm) << 1);
}

bool VertexData::Vertex::operator==(const Vertex& o) const {
    return pos == o.pos && tex == o.tex && norm == o.norm;
}

size_t VertexData::Vertex::stride()
{
    static_assert(sizeof(VertexData::Vertex) == 32);  // sizeof(float) * 8, {vec3 pos, vec2 uv, vec3 norm}
    return sizeof(VertexData::Vertex);
}


VertexData* VertexData::makeIndexed(const VertexData* nonIndexed)
{
    assert(!nonIndexed->isIndexed());

    std::unordered_map<VertexData::Vertex, uint32_t> vertexMap;
    VertexData* indexed = new VertexData();

    for (const VertexData::Vertex& vert : nonIndexed->m_Vertices)
    {
        auto it = vertexMap.find(vert);
        if (it != vertexMap.end())
        {
            // Vertex already exists in map
            indexed->m_Indices.push_back(it->second);
        }
        else
        {
            // New unique vertex, add to map
            uint32_t index = indexed->m_Vertices.size();
            vertexMap[vert] = index;
            indexed->m_Vertices.push_back(vert);
            indexed->m_Indices.push_back(index);
        }
    }
    return indexed;
}