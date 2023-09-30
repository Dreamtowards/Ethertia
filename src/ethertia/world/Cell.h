
#pragma once

#include <glm/vec3.hpp>

#include <ethertia/util/Math.h>

class Material; // tmp

class Cell
{
public:

	float value = 0;  // SDF value, 0 -> on surface, positive -> void, negative -> solid.
	Material* mtl = nullptr;

	glm::vec3 fp{Math::Inf};   // cached FeaturePoint, Inf=Invalid.
	glm::vec3 norm; // cached Normal. Inf=Invalid.

	bool IsSolid() const { return value > 0; }

	bool IsFpValid() const {
		return std::isfinite(fp.x);
	}

	static Cell& Nil()
	{
		static Cell _C{};
		_C.value = -Math::Inf;
		_C.mtl = nullptr;
		return _C;
	}
};