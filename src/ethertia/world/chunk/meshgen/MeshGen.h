

#pragma once

#include <ethertia/render/VertexData.h>
#include <ethertia/world/Chunk.h>


class MeshGen
{
public:

	static void GenerateMesh(Chunk* chunk, VertexData* vtx);

};