//
// Created by Dreamtowards on 2023/1/13.
//

#ifndef ETHERTIA_BIOME_H
#define ETHERTIA_BIOME_H

#include <vector>
#include <iostream>

#include <glm/vec3.hpp>


// Holdridge (1947, 1964)       life zones  https://en.wikipedia.org/wiki/File:Lifezones_Pengo.svg
// Whittaker (1962, 1970, 1975) biome-types https://en.wikipedia.org/wiki/File:Climate_influence_on_terrestrial_biome.svg
// earth vegetation https://en.wikipedia.org/wiki/File:Vegetation.png

class Biome
{
public:

    inline static std::vector<Biome*> BIOMES;

    int m_Id = 0;
    const char* m_Name = nullptr;

    float m_Temperature = 0;  // average annual / bio-temperature ('C) ~: -10 - 35
    float m_Humidity = 0;     // [0, 1]

    // cancelled, use Humidity instead. some biomes wouldn't rain e.g. Hell,Cave and the mm-perc's relation have a little complex.
    // float m_Precipitation = 0;   // annual precipitation (mm):  62 - 16,000
    // float m_Evapotranspiration = 0;

    // color of Grass, Foliage, Water

    Biome(const char* _name, float temp, float humid) : m_Name(_name), m_Temperature(temp), m_Humidity(humid) {
        m_Id = BIOMES.size();
        BIOMES.push_back(this);

        std::cout << "Biome " << m_Id << " " << m_Name << "\n";
    }





    static void find(float temp, float humid) {


    }

};

class Biomes {

    // decl out of the Biome class. for avoid incomplete-class error. there wanted define aside decl.
    inline static Biome
            TUNDRA{"tundra", 1.5, 0.2},
            TAIGA {"taiga", 5.2, 0.3},
            FOREST{"forest", 17.0, 0.4},
            DESERT{"desert", 24.0, 0.1},
            JUNGLE{"jungle", 21.0, 0.8};

    // Plains, Savanna, Birch, Swamp, Cherry Blossom Forest,
    // Orchard, Meadow, Prairie
    // Tropical Rainforest, Temperate Rainforest

    // Plateau
};

#endif //ETHERTIA_BIOME_H
