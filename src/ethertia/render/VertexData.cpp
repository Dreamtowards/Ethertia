//
// Created by Dreamtowards on 2023/4/27.
//

#include "VertexData.h"

#include <ethertia/util/Assert.h>
#include <ethertia/util/Log.h>



VertexData::~VertexData()
{
    Log::info("Delete VertexData: {} with {} vertices", m_Filename, Vertices.size());
}


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

size_t std::hash<VertexData::Vertex>::operator()(const VertexData::Vertex& vertex) const
{
    return ((hash<glm::vec3>()(vertex.pos) ^
            (hash<glm::vec2>()(vertex.tex) << 1)) >> 1) ^
            (hash<glm::vec3>()(vertex.norm)<< 1);
}

#include <unordered_map>

VertexData* VertexData::MakeIndexed(const VertexData* nonIndexed, VertexData* out)
{
    ET_ASSERT(!nonIndexed->IsIndexed());

    std::unordered_map<VertexData::Vertex, uint32_t> vertexMap;

    int nVert = nonIndexed->VertexCount();
    out->Indices.reserve(nVert);
    out->Vertices.reserve(nVert / 6);

    for (const VertexData::Vertex& vert : nonIndexed->Vertices)
    {
        auto it = vertexMap.find(vert);
        if (it != vertexMap.end())
        {
            // Vertex already exists in map
            out->Indices.push_back(it->second);
        }
        else
        {
            // New unique vertex, add to map
            uint32_t index = out->Vertices.size();
            vertexMap[vert] = index;
            out->Vertices.push_back(vert);
            out->Indices.push_back(index);
        }
    }
    return out;
}