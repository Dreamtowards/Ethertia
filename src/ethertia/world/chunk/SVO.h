
#pragma once

// https://eisenwave.github.io/voxel-compression-docs/svo/svo.html


class SVO
{
public:
	inline static const glm::vec3 VERT[8]{
		{0, 0, 0},
		{1, 0, 0},
		{0, 1, 0},
		{1, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{0, 1, 1},
		{1, 1, 1}
	};

	SVO* children[8]{};
};

class SVOLeaf
{
	Chunk* chunk;
};

