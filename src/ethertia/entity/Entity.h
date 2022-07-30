//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_ENTITY_H
#define ETHERTIA_ENTITY_H

#include <glm/vec3.hpp>

class Entity
{
public:
    glm::vec3 position;

    glm::vec3 velocity;

    std::string name;

    glm::vec3 prevposition;
    glm::vec3 intpposition;


};

#endif //ETHERTIA_ENTITY_H
