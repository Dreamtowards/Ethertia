//
// Created by Dreamtowards on 2023/3/25.
//

#ifndef ETHERTIA_WORLD_H
#define ETHERTIA_WORLD_H

#include <vector>

class World
{
public:




private:
    // Loaded Entities
    std::vector<Entity*> m_Entities;

    // Loaded Chunks
    std::unordered_map<glm::vec3, Chunk*> m_Chunks;

};


#endif //ETHERTIA_WORLD_H
