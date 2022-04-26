//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKGENERATOR_H
#define ETHERTIA_CHUNKGENERATOR_H

#include <glm/vec3.hpp>

#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/world/gen/NoiseGeneratorPerlin.h>
#include <ethertia/world/gen/NoiseGeneratorOctaves.h>
#include <ethertia/util/Log.h>
#include <glm/gtx/string_cast.hpp>

#define samplesY 4

//[0] = {double} 152.46242676667106  sY=4
//[1] = {double} 148.83489146911964
//[2] = {double} 162.6399460655372
class ChunkGenerator
{
    NoiseGeneratorPerlin noise{};

public:

    static int sampIdx(int sX, int sZ, int sY) {
        int samplesXZ = 4+1;
        return (sX * samplesXZ + sZ) * (samplesY+1) + sY;
    }

    // 4,16 / 4,8; Temperatured.
    //0 = 468.99357945341194
    //1 = 414.08969663019724
    //16 = -10.0
    //17 = 471.0110929729797

    // FixedTemperature
    //0 = 483.2883515805149
    //1 = 426.71314619021217
    //2 = 374.0023265073945
    //3 = 317.04080700192543
    //4 = 261.58526955368364
    //5 = 199.5673297713375
    Chunk* generateChunk(glm::vec3 chunkpos)
    {
        Chunk* chunk = new Chunk();
        chunk->position = chunkpos;

        Log::info("Gen Terr "+glm::to_string(chunkpos));

        int samplesXZ = 4;
        int sampSizeXZ = 4;
        int sampSizeY = 4;

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
                    double ax = cX*samplesXZ + i;
                    double az = cZ*samplesXZ + j;
                    double ay = cY*samplesY + l;
                    double f = octave6_10.fbm(ax/14, ay/12, az/14);

                    double p = octave4_4.fbm(ax/8, az/8);
                    p = p-ay/10;

                    p += f* (p<0?12:1);

                    samps[tmpi++] = p;
                }
            }
        }

        if (chunkpos.x == 0 && chunkpos.z == 0) {

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

        for(int sX = 0; sX < samplesXZ; ++sX) {
            for(int sZ = 0; sZ < samplesXZ; ++sZ) {
                for(int sY = 0; sY < samplesY; ++sY) {
                    double sp000sum = samps[sampIdx(sX, sZ, sY)];
                    double sp010sum = samps[sampIdx(sX, sZ+1, sY)];
                    double sp100sum = samps[sampIdx(sX+1, sZ, sY)];
                    double sp110sum = samps[sampIdx(sX+1, sZ+1, sY)];
                    double sp001Ydiff = (samps[sampIdx(sX, sZ, sY+1)] - sp000sum) / sampSizeY;
                    double sp011Ydiff = (samps[sampIdx(sX, sZ+1, sY+1)] - sp010sum) / sampSizeY;
                    double sp101Ydiff = (samps[sampIdx(sX+1, sZ, sY+1)] - sp100sum) / sampSizeY;
                    double sp111Ydiff = (samps[sampIdx(sX+1, sZ+1, sY+1)] - sp110sum) / sampSizeY;

                    for(int dY = 0; dY < sampSizeY; ++dY) {
                        double spXsum0 = sp000sum;
                        double spXsum1 = sp010sum;
                        double spXdiffz0 = (sp100sum - sp000sum) / sampSizeXZ;
                        double spXdiffz1 = (sp110sum - sp010sum) / sampSizeXZ;

                        for(int dX = 0; dX < sampSizeXZ; ++dX) {  // X
                            double spZsum = spXsum0;
                            double spZdiffx0 = (spXsum1 - spXsum0) / sampSizeXZ;

                            for(int dZ = 0; dZ < sampSizeXZ; ++dZ) {  // Z   little fill.  one sample-area is 4*4*8=128
                                //double tmpera = temperature[(sX * sampSizeXZ + dX) * 16 + sZ * sampSizeXZ + dZ];
                                int block = 0;
//                                if (sY * sampSizeY + dY < seaLevel) {
//                                    if (tmpera < 0.5D && sY * sampSizeY + dY >= seaLevel - 1) {
//                                        block = Block.ice.blockID;
//                                    } else {
//                                        block = Block.waterStill.blockID;
//                                    }
//                                }

                                if (spZsum > 0.0) {
                                    chunk->setBlock(sX*sampSizeXZ+dX, sY*sampSizeY+dY, sZ*sampSizeXZ+dZ, 1);
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

    NoiseGeneratorOctaves octave1_16;
    NoiseGeneratorOctaves octave2_16;
    NoiseGeneratorOctaves octave3_8;
    NoiseGeneratorOctaves octave4_4;
    NoiseGeneratorOctaves octave5_4;
    NoiseGeneratorOctaves octave6_10;
    NoiseGeneratorOctaves octave7_16;

    static const long SEED_Glacier = 1772835215;

    ChunkGenerator() {
        JRand tmp(0);
        noise.init(tmp);

        JRand jr(SEED_Glacier);
        octave1_16.init(jr, 16);
        octave2_16.init(jr, 16);
        octave3_8.init(jr, 8);
        octave4_4.init(jr, 4);
        octave5_4.init(jr, 4);
        octave6_10.init(jr, 10);
        octave7_16.init(jr, 16);

    }

    void terrgenNoiseGen(double samps[], int csX, int csY, int csZ, int numSampX, int numSampY, int numSampZ) {

        float mXYZ = 684.412;// * (1/0.176);
//        double[] temperature = this.worldObj.getWorldChunkManager().temperature;
//        double[] humidity = this.worldObj.getWorldChunkManager().humidity;


        int n25 = numSampX*numSampZ;
        double terrgenNoise25_1[25] = {};
        double terrgenNoise25_2[25] = {};
        int n425 = numSampX*numSampY*numSampZ;
        double terrgenNoise425_Small[75] = {};
        double terrgenNoise425_1[75] = {};
        double terrgenNoise425_2[75] = {};

        //0 = 181.31595377384392
        //1 = 181.24718370470026
        //2 = 178.61862105942805
        //3 = 174.32236681424385
        //4 = 172.66574945181642
        //5 = 183.2675942699258
        octave6_10.generateNoiseOctavesXZ(terrgenNoise25_1, csX, csZ, numSampX, numSampZ, 1.121f, 1.121f);

        //0 = 5599.223013393779
        //1 = 5842.709574436591
        //2 = 5775.155435070777
        //3 = 5611.82533512294
        //4 = 5777.362012934464
        //5 = 5285.725040499208
        octave7_16.generateNoiseOctavesXZ(terrgenNoise25_2, csX, csZ, numSampX, numSampZ, 200.0f, 200.0f);

        //0 = -37.719004591311624
        //1 = -36.94655349240814
        //2 = -29.194860201798605
        //3 = -37.41867530527445
        //4 = -40.02868728536161
        //5 = -44.86501048132111
        octave3_8.generateNoiseOctaves(terrgenNoise425_Small, csX, csY, csZ, numSampX, numSampY, numSampZ, mXYZ / 80.0f, mXYZ / 160.0f, mXYZ / 80.0f);

        //0 = 17913.852246471666
        //1 = 15783.613101272442
        //2 = 15631.939438305551
        //3 = 13303.907466141165
        //4 = 11746.938307277132
        //5 = 6830.019153351688
        octave1_16.generateNoiseOctaves(terrgenNoise425_1, csX, csY, csZ, numSampX, numSampY, numSampZ, mXYZ, mXYZ, mXYZ);

        //0 = -4289.958717778405
        //1 = -6484.245125849067
        //2 = -6039.228735955636
        //3 = -9144.90736530504
        //4 = -12614.665731291629
        //5 = -12312.85182606164
        octave2_16.generateNoiseOctaves(terrgenNoise425_2, csX, csY, csZ, numSampX, numSampY, numSampZ, mXYZ, mXYZ, mXYZ);
        int idx = 0;
        int idxXZ = 0;
        int sampSizeXZ = 4;

        if (csX == 0 && csZ == 0) {

        }

        for(int sX = 0; sX < numSampX; ++sX) {
            int sXidx = sX * sampSizeXZ + sampSizeXZ / 2;

            for(int sZ = 0; sZ < numSampZ; ++sZ) {
                int sZidx = sZ * sampSizeXZ + sampSizeXZ / 2;
                double tempera = 0.96f;// temperature[sXidx * 16 + sZidx];
                double humidi = 0.16f * tempera;//humidity[sXidx * 16 + sZidx] * tempera;
                double oneMinsHumidi = 1.0f - humidi;
                oneMinsHumidi *= oneMinsHumidi;
                oneMinsHumidi *= oneMinsHumidi;
                oneMinsHumidi = 1.0f - oneMinsHumidi;
                double spXZ1 = (terrgenNoise25_1[idxXZ] + 256) / 512;
                spXZ1 *= oneMinsHumidi;
                if (spXZ1 > 1.0) {
                    spXZ1 = 1.0;
                }

                double spXZ2 = terrgenNoise25_2[idxXZ] / 8000;
                if (spXZ2 < 0.0) {
                    spXZ2 = -spXZ2 * 0.3f;
                }

                spXZ2 = spXZ2 * 3.0f - 2.0f;
                if (spXZ2 < 0.0) {
                    spXZ2 /= 2.0;
                    if (spXZ2 < -1.0) {
                        spXZ2 = -1.0;
                    }
                    spXZ2 /= 2.8;
                    spXZ1 = 0.0;
                } else {
                    if (spXZ2 > 1.0) {
                        spXZ2 = 1.0;
                    }
                    spXZ2 /= 8.0;
                }

                if (spXZ1 < 0.0) {
                    spXZ1 = 0.0;
                }

                spXZ1 += 0.5;
                spXZ2 = spXZ2 * (double)17 / 16.0f;
                double spXZ2_nY = (double)17 / 2.0f + spXZ2 * 4.0f;
                ++idxXZ;

                for(int sY = 0; sY < numSampY; ++sY) {
                    double f = 0.0;
                    double var36 = ((double)(csY*samplesY+sY) - (double)spXZ2_nY) * 12.0f / spXZ1;
                    if (var36 < 0.0) {
                        var36 *= 4.0;
                    }

                    double a = terrgenNoise425_1[idx] / 512;
                    double b = terrgenNoise425_2[idx] / 512;
                    double t = (terrgenNoise425_Small[idx] / 10 + 1) / 2;
                    t = Mth::clamp(t, 0.0, 1.0);
                    f = Mth::lerp(t, a, b);

                    f -= var36;
//                    if (sY > 17.0 - 4) {
//                        double var44 = ((double)sY - ((double)17.0 - 4)) / 3.0F;
//                        f = f * (1.0f - var44) + -10.0f * var44;
//                    }

                    samps[idx++] = f;
                }
            }
        }

    }
};

#endif //ETHERTIA_CHUNKGENERATOR_H
