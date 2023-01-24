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
    glm::vec3 velocity = {0,0,0};

    Texture* texture = nullptr;

    float size = 1;

    float max_lifetime = 0;

    float lifetime = 0;

    Particle(const glm::vec3& position = {0,0,0}, Texture* texture = nullptr, float size = 1, float maxLifetime = 10) : position(position),
                                                                                           texture(texture), size(size),
                                                                                           max_lifetime(maxLifetime) {}

    // return false: need to be delete. out of lifetime
    bool update(float dt)
    {
        velocity += glm::vec3(0, -9.81, 0) * dt;

        position += velocity * dt;

        lifetime += dt;

        return lifetime <= max_lifetime;
    }

};

#endif //ETHERTIA_PARTICLE_H
