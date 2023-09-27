

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




#define ET_OP_VEC3(op, v) { op(v.x), op(v.y), op(v.z) }

	static glm::ivec3 Floor16(glm::ivec3 p)
	{
		return ET_OP_VEC3(Math::Floor16, p);
	}

	static glm::ivec3 Mod16(glm::ivec3 p)
	{
		return ET_OP_VEC3(Math::Mod16, p);
	}



};





//glm::vec3 operator+(const )
//{
//
//}


