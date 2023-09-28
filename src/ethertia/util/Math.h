

#pragma once

#include <cmath>
#include <glm/glm.hpp>

class Math
{
public:

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

};





//glm::vec3 operator+(const )
//{
//
//}


