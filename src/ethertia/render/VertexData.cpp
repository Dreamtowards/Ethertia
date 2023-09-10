//
// Created by Dreamtowards on 2023/4/27.
//

#include "VertexData.h"




//const void* VertexData::data() const {
//    return Vertices.data();
//}
//size_t VertexData::size() const {
//    return sizeof(Vertices[0]) * Vertices.size();
//}

const void* VertexData::vtx_data() const {
    return Vertices.data();
}
size_t VertexData::vtx_size() const {
    return sizeof(Vertices[0]) * Vertices.size();
}

const void* VertexData::idx_data() const {
    return Indices.data();
}
size_t VertexData::idx_size() const {
    return sizeof(Indices[0]) * Indices.size();
}


std::span<const char> VertexData::vtx_span() const
{
    return {(const char*)vtx_data(), vtx_size()};
}
std::span<const char> VertexData::idx_span() const
{
    return {(const char*)idx_data(), idx_size()};
}


uint32_t VertexData::vertexCount() const
{
    return isIndexed() ? Indices.size() : Vertices.size();
}

bool VertexData::isIndexed() const
{
    return !Indices.empty();
}

const VertexData::Vertex& VertexData::vert(uint32_t i) const
{
    return isIndexed() ? Vertices[Indices[i]] : Vertices[i];
}


void VertexData::addVertex(const Vertex& vert)
{
    if (isIndexed())
    {
        Indices.push_back(Vertices.size());
    }

    Vertices.emplace_back(vert);
}






#include <ethertia/util/Log.h>



VertexData::VertexData() {}

VertexData::~VertexData()
{
    Log::info("Delete VertexData: {} with {} vertices", m_Filename, Vertices.size());
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


VertexData* VertexData::MakeIndexed(const VertexData* nonIndexed)
{
    assert(!nonIndexed->isIndexed());

    std::unordered_map<VertexData::Vertex, uint32_t> vertexMap;
    VertexData* indexed = new VertexData();

    int nVert = nonIndexed->vertexCount();
    indexed->Indices.reserve(nVert);
    indexed->Vertices.reserve(nVert / 6);

    for (const VertexData::Vertex& vert : nonIndexed->Vertices)
    {
        auto it = vertexMap.find(vert);
        if (it != vertexMap.end())
        {
            // Vertex already exists in map
            indexed->Indices.push_back(it->second);
        }
        else
        {
            // New unique vertex, add to map
            uint32_t index = indexed->Vertices.size();
            vertexMap[vert] = index;
            indexed->Vertices.push_back(vert);
            indexed->Indices.push_back(index);
        }
    }
    return indexed;
}