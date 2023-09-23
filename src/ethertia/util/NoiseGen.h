
#pragma once




#include <FastNoise/FastNoise.h>

class NoiseGen
{
public:
	inline static FastSIMD::eLevel SIMDLevel = FastSIMD::Level_Null;  // i.e. if it's null, it is dyn max

	static void InitSIMD();


	//static std::string SimdLevel(FastSIMD::eLevel lv);
	//static FastSIMD::eLevel SimdLevel(const std::string& str);
};