//
// Created by Dreamtowards on 2022/9/2.
//

#ifndef ETHERTIA_PARTICLERENDERER_H
#define ETHERTIA_PARTICLERENDERER_H

#include <vector>

#include <ethertia/render/particle/Particle.h>
#include <ethertia/render/ShaderProgram.h>
#include <ethertia/render/GlState.h>

class ParticleRenderer
{
public:
    DECL_SHADER(SHADER, ShaderProgram::decl("shaders/particle/particle.{}"));

    inline static std::vector<Particle*> m_Particles;

    inline static VertexArrays* M_PLANE_C_STRIP = nullptr;

    static void init()
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

    static void updateAll(float dt)
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

    static void renderAll()
    {
        SHADER->useProgram();
        SHADER->setViewProjection();

        for (Particle* p : m_Particles)
        {
            render(*p);
        }
    }

    static void render(const Particle& p)
    {
        render(p.texture, p.position, p.size, p.uv_pos(), glm::vec2(1.0f / p.tex_grids));
    }

    static void render(Texture* tex, glm::vec3 position, float size, glm::vec2 uvPos = {0,0}, glm::vec2 uvSize = {1,1})
    {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->texId);

        SHADER->useProgram();

        SHADER->setVector3f("position", position);
        SHADER->setFloat("size", size);

        SHADER->setVector2f("uv_pos", uvPos);
        SHADER->setVector2f("uv_size", uvSize);

        glBindVertexArray(M_PLANE_C_STRIP->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, M_PLANE_C_STRIP->vertexCount);
//        glDrawArraysInstanced();
    }



    // temporary.
//    inline static glm::vec3 _SUN_POS;

    static void renderSunMoonTex(float dayTime)
    {
        // render Sun and Moon img.

        GlState::blendMode(GlState::ADD);

        static Texture* TEX_SUN = Loader::loadTexture("misc/sky/sun.png");
        static Texture* TEX_MOON = Loader::loadTexture("misc/sky/moon.png");

        using glm::vec3;
        vec3 relSunPos = Mth::rot_dir(dayTime * 2*Mth::PI, vec3(0, 0, 1), vec3(0, -1, 0)) * 300.0f;
        // Mth::anglez(worldDayTime * 2*Mth::PI - 0.5f*Mth::PI) * 300.0f;  // -PI/2: DayTime:0 as midnight SunPos instead of Morning.

        vec3 CamPos = Ethertia::getCamera().position;
        vec3 SunPos = CamPos + relSunPos;
        vec3 MoonPos = CamPos - relSunPos;

        render(TEX_SUN, SunPos, 180.0f);
        render(TEX_MOON, MoonPos, 180.0f);
    }

};

#endif //ETHERTIA_PARTICLERENDERER_H
