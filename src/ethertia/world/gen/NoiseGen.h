//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_NOISEGEN_H
#define ETHERTIA_NOISEGEN_H

#include <FastNoise/FastNoise.h>

class NoiseGen
{
public:
    inline static FastSIMD::eLevel g_SIMDLevel = FastSIMD::Level_Null;  // i.e. if it's null, it is dyn max


    static void initSIMD()  // 0.05ms in debug.
    {
        Log::info("Compiled SIMD Levels: {} :: {} (CPU max: {})\1",
                  NoiseGen::FastSIMD_CompiledLevels("/"),
                  NoiseGen::FastSIMD_LevelName(NoiseGen::g_SIMDLevel),
                  NoiseGen::FastSIMD_LevelName(FastSIMD::CPUMaxSIMDLevel()));

        std::cout << " if crash, try downgrade SIMD_Level in 'settings.json'.\n";
        std::cout.flush();

        {
            // Check whether current SIMD Level actually supported. it will crash if not supported.
            float _test;
            NoiseGen::Perlin()->GenUniformGrid3D(&_test, 0, 0, 0, 1, 1, 1, 0.1, 123);
        }
    }

    static const FastNoise::SmartNode<FastNoise::FractalFBm> NewFractalFBM() {
        return FastNoise::New<FastNoise::FractalFBm>(g_SIMDLevel);
    }

    static const FastNoise::SmartNode<FastNoise::Perlin>& Perlin() {
        static FastNoise::SmartNode<FastNoise::Perlin> perlin = FastNoise::New<FastNoise::Perlin>(g_SIMDLevel);
        return perlin;
    }
    static const FastNoise::SmartNode<FastNoise::Simplex>& Simplex() {
        static FastNoise::SmartNode<FastNoise::Simplex> simplex = FastNoise::New<FastNoise::Simplex>(g_SIMDLevel);
        return simplex;
    }

    template<typename NoiseType>
    static const FastNoise::SmartNode<NoiseType>& Generator() {
        static FastNoise::SmartNode<NoiseType> _inst = FastNoise::New<NoiseType>(g_SIMDLevel);
        return _inst;
    }
    static const FastNoise::SmartNode<FastNoise::CellularValue>& CellularValue() {
        return Generator<FastNoise::CellularValue>();
    }


    inline static int IdxXZ(int rx, int rz) {
        return rz*16 + rx;
    }
    inline static int Idx3(int rx, int ry, int rz) {
        return rz*256+ry*16+rx;
    }




    static std::string FastSIMD_CompiledLevels(const std::string& join = ", ") {
        std::stringstream ss;
        FastSIMD::Level_BitFlags comp = FastSIMD::COMPILED_SIMD_LEVELS;
        for (int i = 0; i < 32; ++i) {
            FastSIMD::Level_BitFlags lev = 1 << i;
            if ((comp & lev) != 0) {
                if (ss.tellp() > 0) { ss << join; }
                ss << FastSIMD_LevelName((FastSIMD::eLevel)lev);
            }
        }
        return ss.str();
    }

    static std::string FastSIMD_LevelName(FastSIMD::eLevel lev) {
        switch (lev) {
            case FastSIMD::Level_Null:  return "";
            case FastSIMD::Level_Scalar:return "Scalar";
            case FastSIMD::Level_SSE:   return "SSE";
            case FastSIMD::Level_SSE2:  return "SSE2";
            case FastSIMD::Level_SSE3:  return "SSE3";
            case FastSIMD::Level_SSSE3: return "SSSE3";
            case FastSIMD::Level_SSE41: return "SSE41";
            case FastSIMD::Level_SSE42: return "SSE42";
            case FastSIMD::Level_AVX:   return "AVX";
            case FastSIMD::Level_AVX2:  return "AVX2";
            case FastSIMD::Level_AVX512:return "AVX512";
            case FastSIMD::Level_NEON:  return "NEON";
            default: return "Unknown";
        }
    }
    static FastSIMD::eLevel FastSIMD_ofLevelName(const std::string& n) {
        if (n == "Scalar") return FastSIMD::Level_Scalar;
        if (n == "SSE")    return FastSIMD::Level_SSE;
        if (n == "SSE2")   return FastSIMD::Level_SSE2;
        if (n == "SSE3")   return FastSIMD::Level_SSE3;
        if (n == "SSSE3")  return FastSIMD::Level_SSSE3;
        if (n == "SSE41")  return FastSIMD::Level_SSE41;
        if (n == "SSE42")  return FastSIMD::Level_SSE42;
        if (n == "AVX")    return FastSIMD::Level_AVX;
        if (n == "AVX2")   return FastSIMD::Level_AVX2;
        if (n == "AVX512") return FastSIMD::Level_AVX512;
        if (n == "NEON")   return FastSIMD::Level_NEON;
        if (n != "") {
            Log::warn("Unknown SIMD Level name {}", n);
        }
        return FastSIMD::Level_Null;
    }
};

#endif //ETHERTIA_NOISEGEN_H
