
#pragma once

#include <glm/vec3.hpp>

#include <ethertia/util/Math.h>

class Material;

class Cell
{
public:

	/// SDF value, used for Isosurface Extraction.
	/// 0 -> surface, +0 positive -> void, -0 negative -> solid.
	float value = 0;  // -inf means nil cell.
	// TODO: int8 m_Value; float value() => mValue / (float)INT8;

	Material* mtl = nullptr;
	// TODO: uint16 MtlId

	glm::vec3 fp{ Math::Inf };	// cached FeaturePoint, Inf=Invalid.  [0, 1]
	glm::vec3 norm;				// cached Normal.		Inf=Invalid.  [-1, 1]  if Fp invalid then Norm also means invalid.


	bool IsSolid() const { return value > 0; }

	bool IsNil() const { return value == -Math::Inf; }

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