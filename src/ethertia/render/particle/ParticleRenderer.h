//
// Created by Dreamtowards on 2022/9/2.
//

#ifndef ETHERTIA_PARTICLERENDERER_H
#define ETHERTIA_PARTICLERENDERER_H

#include <vector>

#include <ethertia/render/particle/Particle.h>
#include <ethertia/render/shader/ShaderProgram.h>
#include <ethertia/render/GlState.h>

class ParticleRenderer
{
public:

    ShaderProgram m_ParticleShader =
            Loader::loadShaderProgram("shaders/particle/particle.{}", false);

    inline static std::vector<Particle*> m_Particles;

    inline static Model* M_PLANE_C_STRIP = nullptr;

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
    }

    void updateAll(float dt)
    {
        std::vector<Particle*> tobeDelete;

        for (Particle* p : m_Particles)
        {
            if (!p->update(dt))
            {
                tobeDelete.push_back(p);
            }
        }

        // Delete particles that out of lifetime.
        for (Particle* p : tobeDelete)
        {
            Collections::erase(m_Particles, p);
            delete p;
        }
    }

    void renderAll()
    {
        m_ParticleShader.useProgram();
        m_ParticleShader.setViewProjection();

        for (Particle* p : m_Particles)
        {
            render(*p);
        }
    }

    void render(const Particle& p)
    {
        render(p.texture, p.position, p.size, p.uv_pos(), glm::vec2(1.0f / p.tex_grids));
    }

    void render(Texture* tex, glm::vec3 position, float size, glm::vec2 uvPos = {0,0}, glm::vec2 uvSize = {1,1})
    {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->texId);

        m_ParticleShader.useProgram();

        m_ParticleShader.setVector3f("position", position);
        m_ParticleShader.setFloat("size", size);

        m_ParticleShader.setVector2f("uv_pos", uvPos);
        m_ParticleShader.setVector2f("uv_size", uvSize);

        glBindVertexArray(M_PLANE_C_STRIP->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, M_PLANE_C_STRIP->vertexCount);
//        glDrawArraysInstanced();
    }



    // temporary.
    inline static glm::vec3 _SUN_POS;

    void renderSunMoonTex(float worldDayTime)
    {
        // render Sun and Moon img.

        GlState::blendMode(GlState::ADD);

        static Texture* TEX_SUN = Loader::loadTexture("misc/sky/sun.png");
        static Texture* TEX_MOON = Loader::loadTexture("misc/sky/moon.png");

        using glm::vec3;
        // -PI/2: DayTime:0 as midnight SunPos instead of Morning.
        // glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1)) * glm::vec4(1, 0, 0, 1.0);
        vec3 relSunPos = Mth::anglez(worldDayTime * 2*Mth::PI - 0.5f*Mth::PI) * 300.0f;

        vec3 CamPos = Ethertia::getCamera()->position;
        vec3 SunPos = CamPos + relSunPos;
        vec3 MoonPos = CamPos - relSunPos;
        _SUN_POS = SunPos;

        render(TEX_SUN, SunPos, 180.0f);
        render(TEX_MOON, MoonPos, 180.0f);
    }

};

#endif //ETHERTIA_PARTICLERENDERER_H
