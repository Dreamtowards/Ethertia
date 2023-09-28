
#pragma once

#include <FastNoise/FastNoise.h>

#include <string_view>

#include <chrono>

class NoiseGen
{
public:
	inline static FastSIMD::eLevel SIMDLevel = FastSIMD::Level_Null;  // i.e. if it's null, it is dyn max

	static void InitSIMD();

	static const char* SIMDLevel_str(FastSIMD::eLevel lv);
	static FastSIMD::eLevel SIMDLevel_val(std::string_view str);


	template<typename NoiseType>
	static const FastNoise::SmartNode<NoiseType>& GenNode() {
		static FastNoise::SmartNode<NoiseType> _inst = FastNoise::New<NoiseType>(SIMDLevel);
		return _inst;
	}

	static const FastNoise::SmartNode<FastNoise::FractalFBm> NewFractalFBM() {
		return FastNoise::New<FastNoise::FractalFBm>(SIMDLevel);
	}

	static const FastNoise::SmartNode<FastNoise::Perlin>& Perlin() {
		return NoiseGen::GenNode<FastNoise::Perlin>();
	}
	static const FastNoise::SmartNode<FastNoise::Simplex>& Simplex() {
		return NoiseGen::GenNode<FastNoise::Simplex>();
	}
};