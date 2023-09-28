

#include "MeshGen.h"

#include <stdx/stdx.h>
#include <ethertia/world/Chunk.h>



#pragma region Blocky Cube


static float CUBE_POS[] = {
        0, 0, 1, 0, 1, 1, 0, 1, 0,  // Left -X
        0, 0, 1, 0, 1, 0, 0, 0, 0,
        1, 0, 0, 1, 1, 0, 1, 1, 1,  // Right +X
        1, 0, 0, 1, 1, 1, 1, 0, 1,
        0, 0, 1, 0, 0, 0, 1, 0, 0,  // Bottom -Y
        0, 0, 1, 1, 0, 0, 1, 0, 1,
        0, 1, 1, 1, 1, 1, 1, 1, 0,  // Bottom +Y
        0, 1, 1, 1, 1, 0, 0, 1, 0,
        0, 0, 0, 0, 1, 0, 1, 1, 0,  // Front -Z
        0, 0, 0, 1, 1, 0, 1, 0, 0,
        1, 0, 1, 1, 1, 1, 0, 1, 1,  // Back +Z
        1, 0, 1, 0, 1, 1, 0, 0, 1,
};
static float CUBE_UV[] = {
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,  // One Face.
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
};
static float CUBE_NORM[] = {
        -1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
        0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1,
        0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1
};

static void _PutCube(VertexData& vtx, glm::ivec3 lp, Chunk& chunk, Cell& cell)
{
    for (int face_i = 0; face_i < 6; ++face_i)
    {
        glm::ivec3 face_dir = stdx::cast<glm::vec3>(&CUBE_NORM[face_i * 18]);   // 18: 3 scalar * 3 vertex * 2 triangle

        Cell neibCell = chunk.GetCell(lp + face_dir, true);
        if (!neibCell.IsSolid())  // !IsOpaque()
        {
            for (int vert_i = 0; vert_i < 6; ++vert_i)
            {
                vtx.AddVertex({
                    stdx::cast<glm::vec3>(&CUBE_POS[face_i * 18 + vert_i * 3]) + glm::vec3(lp),
                    {0, 0},  // stdx::cast<glm::vec2>(&CUBE_UV[face_i * 12 + vert_i * 2]),
                    stdx::cast<glm::vec3>(&CUBE_NORM[face_i * 18 + vert_i * 3])
                    });
            }
        }
    }
}



#pragma endregion


void MeshGen::GenerateMesh(Chunk& chunk, VertexData& vtx)
{

    for (int rx = 0; rx < 16; ++rx)
    {
        for (int ry = 0; ry < 16; ++ry)
        {
            for (int rz = 0; rz < 16; ++rz)
            {
                glm::ivec3 lp{rx, ry, rz};
                Cell& cell = chunk.LocalCell(lp);

                if (cell.IsSolid())
                {
                    _PutCube(vtx, lp, chunk, cell);
                }
            }
        }
    }

}