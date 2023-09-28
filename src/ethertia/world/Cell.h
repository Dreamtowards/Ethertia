
#pragma once

#include <glm/vec3.hpp>
#include <cmath>

class Material; // tmp

class Cell
{
public:

	float value = 0;  // SDF value, 0 -> on surface, positive -> void, negative -> solid.
	Material* mtl = nullptr;

	glm::vec3 p;    // cached FeaturePoint, Inf=Invalid.
	glm::vec3 norm; // cached Normal. Inf=Invalid.

	bool IsSolid() const { return value > 0; }

	static Cell& Nil()
	{
		static Cell _C{};
		_C.value = std::numeric_limits<float>::infinity();
		_C.mtl = nullptr;
		return _C;
	}
};