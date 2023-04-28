//
// Created by Dreamtowards on 2023/4/27.
//



#include "VertexData.h"




const void* VertexData::data() const
{
    return m_Vertices.data();
}

size_t VertexData::size() const
{
    static_assert(sizeof(VertexData::Vertex) == sizeof(m_Vertices[0]));
    return sizeof(VertexData::Vertex) * m_Vertices.size();
}

size_t VertexData::vertexCount() const
{
    return isIndexed() ? m_Indices.size() : m_Vertices.size();
}

bool VertexData::isIndexed() const
{
    return !m_Indices.empty();
}







#include <ethertia/util/Log.h>

VertexData::VertexData()
{
    Log::info("New VertexData");
}
VertexData::~VertexData()
{
    Log::info("Delete VertexData: {} with {} vertices", m_Filename, m_Vertices.size());
}









#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {
    template<> struct hash<VertexData::Vertex> {
        size_t operator()(VertexData::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                    (hash<glm::vec2>()(vertex.tex)  << 1)) >> 1) ^
                    (hash<glm::vec3>()(vertex.norm) << 1);
        }
    };
}

bool VertexData::Vertex::operator==(const Vertex& o) const {
    return pos == o.pos && tex == o.tex && norm == o.norm;
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