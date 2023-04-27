//
// Created by Dreamtowards on 2023/4/27.
//


#include "VertexData.h"

#include <unordered_map>


const void* VertexData::data() const
{
    return m_Vertices.data();
}

size_t VertexData::size() const
{
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