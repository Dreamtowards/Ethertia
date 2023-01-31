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
    int tex_grids = 1;

    float size = 1;

    // size grow per second
    float size_grow = 0;

    float max_lifetime = 5;

    float lifetime = 0;

//    Particle(const glm::vec3& position, Texture* texture, float size, float maxLifetime) : position(position),
//                                                                                           texture(texture), size(size),
//                                                                                           max_lifetime(maxLifetime) {}

    // return false: need to be delete. out of lifetime
    bool update(float dt)
    {
        velocity += glm::vec3(0, -9.81/5, 0) * dt;

        position += velocity * dt;

        lifetime += dt;

        size += size_grow * dt;

        return lifetime <= max_lifetime;
    }

    glm::vec2 uv_pos() const
    {
        float life_perc = lifetime / max_lifetime;

        int idx_grid = int(life_perc * (tex_grids*tex_grids));

        return {
          (idx_grid % tex_grids) / (float)tex_grids,
          (1.0-1.0/tex_grids) - (idx_grid / tex_grids) / (float)tex_grids
        };
    }

};

#endif //ETHERTIA_PARTICLE_H
