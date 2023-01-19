//
// Created by Dreamtowards on 2022/9/2.
//

#ifndef ETHERTIA_PARTICLERENDERER_H
#define ETHERTIA_PARTICLERENDERER_H

#include <vector>

#include <ethertia/render/Particle.h>
#include <ethertia/render/shader/ShaderProgram.h>

class ParticleRenderer
{
public:

    ShaderProgram m_ParticleShader =
            Loader::loadShaderProgram("shaders/particle/particle.{}", false);

    inline static std::vector<Particle*> m_Particles;

    inline static Model* M_PLANE_C_STRIP = nullptr;

    inline static Particle* SUN  = nullptr;
    inline static Particle* MOON = nullptr;

    ParticleRenderer()
    {
        {
            float pos[] = {
                     0.5, -0.5, 0,  // RB
                     0.5,  0.5, 0,  // RT
                    -0.5, -0.5, 0,  // LB
                    -0.5,  0.5, 0,  // LT
            };
            float tex[] = {
                    1, 0,
                    1, 1,
                    0, 0,
                    0, 1,
            };
            M_PLANE_C_STRIP = Loader::loadModel(4, {
                    {3, pos},
                    {2, tex}
            });
        }

        {
            SUN = new Particle();
            SUN->texture = Loader::loadTexture("misc/sky/sun.png");
            SUN->size = 180;
            m_Particles.push_back(SUN);

            MOON = new Particle();
            MOON->texture = Loader::loadTexture("misc/sky/moon.png");
            MOON->size = 70;
            m_Particles.push_back(MOON);
        }
    }

    static void updateSunMoonPos()
    {
        glm::vec3 relSun = Mth::anglez(Ethertia::getWorld()->m_DayTime * 2*Mth::PI - 0.5f*Mth::PI) * 300.0f;

        // -PI/2: DayTime:0 as midnight SunPos instead of Morning.
        SUN->position = relSun + Ethertia::getCamera()->position;

        MOON->position = -relSun + Ethertia::getCamera()->position;
    }

    void update(float dt)
    {

    }

    void renderAll()
    {
        m_ParticleShader.useProgram();
        m_ParticleShader.setViewProjection();

        for (Particle* p : m_Particles)
        {
            render(p);
        }
    }

    void render(Particle* particle)
    {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particle->texture->texId);

        m_ParticleShader.useProgram();

        m_ParticleShader.setVector3f("position", particle->position);
        m_ParticleShader.setFloat("size", particle->size);

        glBindVertexArray(M_PLANE_C_STRIP->vaoId);
//        glDrawArraysInstanced();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, M_PLANE_C_STRIP->vertexCount);
    }

};

#endif //ETHERTIA_PARTICLERENDERER_H
