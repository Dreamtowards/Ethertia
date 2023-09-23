//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_BIOME_H
#define ETHERTIA_BIOME_H

#include <vector>
#include <iostream>

#include <glm/vec3.hpp>

#include <ethertia/util/Colors.h>


// Holdridge (1947, 1964)       life zones  https://en.wikipedia.org/wiki/File:Lifezones_Pengo.svg
// Whittaker (1962, 1970, 1975) biome-types https://en.wikipedia.org/wiki/File:Climate_influence_on_terrestrial_biome.svg
// earth vegetation https://en.wikipedia.org/wiki/File:Vegetation.png

class Biome
{
public:

    inline static Registry<Biome> REGISTRY;

    const char* m_Name = nullptr;

    // should always in same scale [0, 1] or the 'distance' calculation will not balance
    float m_Temperature = 0;  // [0, 1] average annual / bio-temperature ('C) ~: -10 - 35
    float m_Humidity = 0;     // [0, 1]

    // cancelled, use Humidity instead. some biomes wouldn't rain e.g. Hell,Cave and the mm-perc's relation have a little complex.
    // float m_Precipitation = 0;   // annual precipitation (mm):  62 - 16,000
    // float m_Evapotranspiration = 0;

    // color of Grass, Foliage, Water

    struct Temperature {
        inline static const float C_MIN = -10,
                C_MAX = 30.0;

        static float ofCelsius(float c) {
            return Mth::rlerp(c, C_MIN, C_MAX);
        }
        static float toCelsius(float t) {
            return Mth::lerp(t, C_MIN, C_MAX);
        }
    };

    Biome(const char* _name, float temp_c, float humid) : m_Name(_name), m_Humidity(humid) {
        REGISTRY.regist(this);

        m_Temperature = Biome::Temperature::ofCelsius(temp_c);

    }

    std::string getRegistryId() const {
        return m_Name;
    }


    static Biome* lookup(float temp, float humid, float* out_dist = nullptr)
    {
        float  min_dist = FLT_MAX;
        Biome* min_bio = nullptr;

        for (auto& it : REGISTRY) {
            Biome* bio = it.second;
            float dist = glm::length2(glm::vec2(bio->m_Temperature, bio->m_Humidity) - glm::vec2(temp, humid));
            if (min_dist > dist) {
                min_dist = dist;
                min_bio = bio;
            }
        }

        if (out_dist) {
            *out_dist = min_dist;
        }
        return min_bio;
    }

};

class Biomes {

    // decl out of the Biome class. for avoid incomplete-class error. there wanted define aside decl.
    inline static Biome
            TUNDRA{"tundra", 1.5,  0.2},
            TAIGA {"taiga",  5.2,  0.3},
            FOREST{"forest", 17.0, 0.4},
            DESERT{"desert", 24.0, 0.1},
            JUNGLE{"jungle", 21.0, 0.8};

    // Plains, Savanna, Birch, Swamp, Cherry Blossom Forest,
    // Orchard, Meadow, Prairie
    // Tropical Rainforest, Temperate Rainforest

    // Plateau

public:
    static glm::vec3 color(const Biome* bio) {
        if (bio == &Biomes::TUNDRA) return Colors::AQUA_DARK;
        else if (bio == &Biomes::TAIGA) return Colors::AQUA;
        else if (bio == &Biomes::FOREST) return Colors::GREEN;
        else if (bio == &Biomes::JUNGLE) return Colors::GREEN_DARK;
        else if (bio == &Biomes::DESERT) return Colors::YELLOW;
        else return Colors::RED;
    }

};

#endif //ETHERTIA_BIOME_H
