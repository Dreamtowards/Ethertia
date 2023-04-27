//
// Created by Dreamtowards on 2023/2/12.
//

#ifndef ETHERTIA_SSAORENDERER_H
#define ETHERTIA_SSAORENDERER_H

#include <random>

#include <ethertia/render/deferred/ComposeRenderer.h>
#include <ethertia/render/ShaderProgram.h>
#include <ethertia/render/RenderCommand.h>

class SSAORenderer
{
public:
    DECL_SHADER(SHADER, ShaderProgram::decl("shaders/ssao/ssao.{}"));

    inline static Framebuffer* fboSSAO = nullptr;

    inline static Texture* m_TexNoise = nullptr;

    SSAORenderer() = delete;

    static void init()
    {
        fboSSAO = Framebuffer::GenFramebuffer(720, 480, [](Framebuffer* fbo)
        {
            fbo->attachColorTexture(0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);//, GL_RGBA32F, GL_RGB, GL_FLOAT);
        });

        SHADER->useProgram();
        SHADER->setInt("gPositionDepth", 0);
        SHADER->setInt("gNormal", 1);
        SHADER->setInt("texNoise", 2);

        using glm::vec3;

        std::uniform_real_distribution<float> rand(0.0f, 1.0f);
        std::default_random_engine gen;

        // SSAO Sample Kernel
        int samples = 64;
        for (int i = 0; i < samples; ++i) {
            vec3 samp{
                    rand(gen) * 2.0f - 1.0f,
                    rand(gen) * 2.0f - 1.0f,
                    rand(gen)
            };
            samp = glm::normalize(samp) * rand(gen);

            // close to core.
            float f = float(i) / samples;
            f = std::lerp(0.1f, 1.0f, f * f);
            samp *= f;

            // ssaoKernel.push_back(samp);

            static auto u_KernelSamples = ShaderProgram::_GenArrayNames("kernelSamples[{}]", 64);
            SHADER->setVector3f(u_KernelSamples[i], samp);
        }

        // Rand Noise (for TBN, de-banding)
        std::vector<glm::vec3> ssaoRand;
        for (int i = 0; i < 16; ++i) {
            vec3 noise{
                    rand(gen) * 2.0f - 1.0f,
                    rand(gen) * 2.0f - 1.0f,
                    0.0f
            };
            ssaoRand.push_back(noise);
        }
        m_TexNoise = Loader::loadTexture(4, 4, &ssaoRand[0], GL_RGB16F, GL_RGB, GL_FLOAT);

    }

    static void renderSSAO(Texture* gPositionDepth, Texture* gNormal, bool onlyClear = false)
    {
        auto _ap = fboSSAO->BeginFramebuffer_Scoped();

        RenderCommand::Clear();

        if (onlyClear)
            return;

        gPositionDepth->BindTexture(0);
        gNormal       ->BindTexture(1);
        m_TexNoise    ->BindTexture(2);


        SHADER->useProgram();

        auto& vp = Ethertia::getViewport();  // ! is not framebuffer size.
        SHADER->setVector2f("noiseScale", glm::vec2(vp.width / 4.0f, vp.height / 4.0f));

        SHADER->setViewProjection();

        RenderCommand::DrawFullQuad();
    }

};

#endif //ETHERTIA_SSAORENDERER_H
