//
// Created by Dreamtowards on 2022/9/2.
//

#ifndef ETHERTIA_PARTICLE_H
#define ETHERTIA_PARTICLE_H

#include <vector>

#include <ethertia/render/Texture.h>

class Particle
{
public:

    glm::vec3 position;

    Texture* texture = nullptr;

};

#endif //ETHERTIA_PARTICLE_H
