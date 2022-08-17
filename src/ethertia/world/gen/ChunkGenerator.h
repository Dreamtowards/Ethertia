//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/world/gen/NoiseGeneratorPerlin.h>
#include <ethertia/util/Log.h>
#include <ethertia/init/Blocks.h>


class ChunkGenerator
{
public:
    NoiseGeneratorPerlin noise{};


    ChunkGenerator() {
        noise.initPermutations(1);

    }

    static int sampIdx(int sX, int sZ, int sY) {
        int samples = 4+1;
        return (sX * samples + sZ) * samples + sY;
    }

    Chunk* generateChunk(glm::vec3 chunkpos, World* world) {
        Chunk* chunk = new Chunk();
        chunk->position = chunkpos;
        chunk->world = world;

//        Log::info("Gen Terr "+glm::to_string(chunkpos));

//chunk->setBlock(0,0,0,Blocks::LEAVES);
//        return chunk;

        int samples = 4;
        int sampleSize = 4;

        double samps[125];  // len: numSampXYZ mul. 5*17*5=425; (samplesXZ+1)*(samplesY+1)*(samplesXZ+1)
        int cX = (int)chunkpos.x / 16;
        int cY = (int)chunkpos.y / 16;
        int cZ = (int)chunkpos.z / 16;
//        terrgenNoiseGen(samps, cX*samplesXZ,
//                        cY*samplesY,
//                        cZ*samplesXZ, samplesXZ+1, samplesY+1, samplesXZ+1);

        int tmpi = 0;
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                for (int l = 0; l < 5; ++l) {
                    double ax = cX*samples + i;
                    double az = cZ*samples + j;
                    double ay = cY*samples + l;
                    double f = noise.fbm(ax/14, ay/12, az/14, 4);

                    double p = noise.fbm(ax/8, az/8, 1);
                    p = p-ay/10;

                    p += f* (p<0?12:1);

                    samps[tmpi++] = p;
                }
            }
        }

//        for (int rx = 0; rx < 16; ++rx) {
//            for (int rz = 0; rz < 16; ++rz) {
//                for (int ry = 0; ry < 16; ++ry) {
//                    float x = chunkpos.x + rx;
//                    float y = chunkpos.y + ry;
//                    float z = chunkpos.z + rz;
//                    float f = noise.noise(x / 40, y / 60, z / 40);
//
//                    if (f < 0)
//                        chunk->setBlock(rx, ry, rz, 1);
//
//                }
//            }
//        }

        for(int sX = 0; sX < samples; ++sX) {
            for(int sZ = 0; sZ < samples; ++sZ) {
                for(int sY = 0; sY < samples; ++sY) {
                    double sp000sum = samps[sampIdx(sX, sZ, sY)];
                    double sp010sum = samps[sampIdx(sX, sZ+1, sY)];
                    double sp100sum = samps[sampIdx(sX+1, sZ, sY)];
                    double sp110sum = samps[sampIdx(sX+1, sZ+1, sY)];
                    double sp001Ydiff = (samps[sampIdx(sX, sZ, sY+1)] - sp000sum) / sampleSize;
                    double sp011Ydiff = (samps[sampIdx(sX, sZ+1, sY+1)] - sp010sum) / sampleSize;
                    double sp101Ydiff = (samps[sampIdx(sX+1, sZ, sY+1)] - sp100sum) / sampleSize;
                    double sp111Ydiff = (samps[sampIdx(sX+1, sZ+1, sY+1)] - sp110sum) / sampleSize;

                    for(int dY = 0; dY < sampleSize; ++dY) {
                        double spXsum0 = sp000sum;
                        double spXsum1 = sp010sum;
                        double spXdiffz0 = (sp100sum - sp000sum) / sampleSize;
                        double spXdiffz1 = (sp110sum - sp010sum) / sampleSize;

                        for(int dX = 0; dX < sampleSize; ++dX) {  // X
                            double spZsum = spXsum0;
                            double spZdiffx0 = (spXsum1 - spXsum0) / sampleSize;

                            for(int dZ = 0; dZ < sampleSize; ++dZ) {
//                                if (sY * sampSizeY + dY < seaLevel) {
//                                    if (tmpera < 0.5D && sY * sampSizeY + dY >= seaLevel - 1) {
//                                        block = Block.ice.blockID;
//                                    } else {
//                                        block = Block.waterStill.blockID;
//                                    }
//                                }

                                u8 bl = 0;
                                if (spZsum > 0.0) {
                                    bl = Blocks::STONE;
                                } else if (chunkpos.y+sY*sampleSize+dY < 0) {
                                    bl = Blocks::WATER;
                                }
                                if (bl) {
                                    chunk->setBlock(sX*sampleSize+dX, sY*sampleSize+dY, sZ*sampleSize+dZ, bl);
                                }
                                spZsum += spZdiffx0;
                            }

                            spXsum0 += spXdiffz0;
                            spXsum1 += spXdiffz1;
                        }

                        sp000sum += sp001Ydiff;
                        sp010sum += sp011Ydiff;
                        sp100sum += sp101Ydiff;
                        sp110sum += sp111Ydiff;
                    }
                }
            }
        }


        return chunk;
    }

};

#endif //ETHERTIA_CHUNKGENERATOR_H
