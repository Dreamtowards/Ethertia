
#pragma once

#include <glm/vec3.hpp>

class Material; // tmp

class Cell
{
public:

	float value = 0;  // SDF value, 0 -> on surface, positive -> void, negative -> solid.
	Material* mtl = nullptr;

	glm::vec3 p;    // cached FeaturePoint, Inf=Invalid.
	glm::vec3 norm; // cached Normal. Inf=Invalid.

};