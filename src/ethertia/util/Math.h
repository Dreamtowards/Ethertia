

#pragma once

#include <cmath>
#include <glm/glm.hpp>

class Math
{
public:

	inline static const float Inf = std::numeric_limits<float>::infinity();

	static int Floor(float x)
	{
		return (int)x - ((int)x > x);
	}

	static int Ceil(float x)
	{
		return (int)x + (x > (int)x);
	}

	static int Mod(int x, int n)
	{
		return ((x % n) + n) % n;  // float f = x % n; return f < 0 ? f + n : f;
	}

	static float Mod(float x, float n)
	{
		return x - n * Math::Floor(x / n);
	}

	//static int Floor(float v, int n)
	//{
	//	return Math::Floor(v / n) * n;
	//}

	static int Floor16(int x)
	{
		return x & ~15;
	}

	static unsigned int Mod16(int x)
	{
		return x & 15;
	}




#define ET_MAKE_VEC3(v, op) { op(v.x), op(v.y), op(v.z) }

	static glm::ivec3 Floor16(glm::ivec3 p)
	{
		return ET_MAKE_VEC3(p, Math::Floor16);
	}

	static glm::ivec3 Mod16(glm::ivec3 p)
	{
		return ET_MAKE_VEC3(p, Math::Mod16);
	}

// e.g. ET_VEC3_CMP(dist, <=, viewer_loaddist, &&);
#define ET_VEC3_CMP(a, cmp, b, conn) (a.x cmp b.x conn a.y cmp b.y conn a.z cmp b.z)







	template<typename T>
	static T InverseLerp(T t, T a, T b)
	{
		return (t - a) / (b - a);
	}




	static bool IsFinite(glm::vec3 v)
	{
		return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
	}

	static bool AnyNan(glm::vec3 v)
	{
		return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
	}
	static bool AnyInf(glm::vec3 v)
	{
		return std::isinf(v.x) || std::isinf(v.y) || std::isinf(v.z);
	}

	static bool IsZero(glm::vec3 v)
	{
		return v.x == 0 && v.y == 0 && v.z == 0;
	}
	static bool IsZero(glm::vec2 v)
	{
		return v.x == 0 && v.y == 0;
	}

};

