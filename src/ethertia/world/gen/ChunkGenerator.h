//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <FastNoise/FastNoise.h>

#include <ethertia/init/Materials.h>
#include <ethertia/world/Cell.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/util/Log.h>


class ChunkGenerator
{

public:
    using vec3 = glm::vec3;

    inline static FastSIMD::eLevel g_SIMDLevel = FastSIMD::Level_Null;  // i.e. if it's null, it is dyn max

    static void initSIMD()
    {
        Log::info("Compiled SIMD Levels: {} : CPU {}\1",
                  ChunkGenerator::FastSIMD_CompiledLevels("/"),
                  ChunkGenerator::FastSIMD_LevelName(FastSIMD::CPUMaxSIMDLevel()));

        if (ChunkGenerator::g_SIMDLevel != FastSIMD::Level_Null) {
            std::cout << " :: " << ChunkGenerator::FastSIMD_LevelName(ChunkGenerator::g_SIMDLevel) << ".\n";
        } else {
            std::cout << ".\n";
        }
    }

    static const FastNoise::SmartNode<FastNoise::Perlin>& Perlin() {
        static FastNoise::SmartNode<FastNoise::Perlin> perlin = FastNoise::New<FastNoise::Perlin>(g_SIMDLevel);
        return perlin;
    }
    static const FastNoise::SmartNode<FastNoise::Simplex>& Simplex() {
        static FastNoise::SmartNode<FastNoise::Simplex> simplex = FastNoise::New<FastNoise::Simplex>(g_SIMDLevel);
        return simplex;
    }

    inline static int IdxXZ(int rx, int rz) {
        return rz*16 + rx;
    }
    inline static int Idx3(int rx, int ry, int rz) {
        return rz*256+ry*16+rx;
    }


    ChunkGenerator() {

    }

    void GenChunk_Flat(Chunk* chunk)
    {

        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {
                    int y = chunk->position.y + ry;

                    chunk->setCell(rx,ry,rz, Cell(Materials::STONE, 10 - y));

                }
            }
        }
    }

    Chunk* generateChunk(glm::vec3 chunkpos, World* world) {
        Chunk* chunk = new Chunk(chunkpos, world);
        uint32_t seed = world->m_Seed;

//        GenChunk_Flat(chunk);
//        return chunk;

        auto fnFrac = FastNoise::New<FastNoise::FractalFBm>(g_SIMDLevel);

        fnFrac->SetSource(Perlin());
        fnFrac->SetOctaveCount(10);

        float noiseVal[16 * 16 * 16];  // chunkpos is Block Coordinate,,
        fnFrac->GenUniformGrid3D(noiseVal, chunkpos.x, chunkpos.y, chunkpos.z, 16, 16, 16, 0.01, seed);

        float noiseTerrHeight[16*16];
        fnFrac->GenUniformGrid2D(noiseTerrHeight, chunkpos.x, chunkpos.z, 16, 16, 1 / 200.0f, seed);

        int idxXZY = 0;
        for (int rx = 0; rx < 16; ++rx) {
            for (int ry = 0; ry < 16; ++ry) {
                for (int rz = 0; rz < 16; ++rz) {  // rz*256+ry*16+rx
                    float terr = noiseTerrHeight[IdxXZ(rx, rz)];// - terrRg.min;

                    float x = chunkpos.x + rx,
                          y = chunkpos.y + ry,
                          z = chunkpos.z + rz;

                    float f = terr - y/64.0f;//noiseVal[idxXZY++];

//                    f += terr;

                    u8 mtl = 0;
                    if (f > 0) {
                        mtl = Materials::STONE;
                    } else if (y < 0) {
                        mtl = Materials::WATER;
                    }

                    chunk->setCell(rx,ry,rz, Cell(mtl, f));
                }
            }
        }

        return chunk;
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

#endif //ETHERTIA_CHUNKGENERATOR_H
