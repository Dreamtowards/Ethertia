

#pragma once

#include <cmath>

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

	static int Floor16(int x)
	{
		return x & ~15;
	}

	static unsigned int Mod16(int x)
	{
		return x & 15;
	}

	//static int Floor(float v, int n)
	//{
	//	return Math::Floor(v / n) * n;
	//}

	//static int Mod(int x, int n)
	//{
	//	int f = x % n;
	//	return f < 0 ? f + n : f;
	//}
};