//
// Created by Dreamtowards on 2023/2/12.
//

#ifndef ETHERTIA_SSAORENDERER_H
#define ETHERTIA_SSAORENDERER_H

#include <ethertia/render/compose/ComposeRenderer.h>
#include <ethertia/render/shader/ShaderProgram.h>

class SSAORenderer
{
public:

    inline static ShaderProgram shaderSSAO;
    inline static Framebuffer* fboSSAO = nullptr;

    inline static Texture* m_TexNoise = nullptr;

    SSAORenderer() = delete;

    static void init()
    {
        fboSSAO = Framebuffer::GenFramebuffer(720, 480, [](Framebuffer* fbo)
        {
            fbo->attachColorTexture(0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);//, GL_RGBA32F, GL_RGB, GL_FLOAT);
        });

        shaderSSAO = Loader::loadShaderProgram("shaders/ssao/ssao.{}");

        Log::info("IsProg", (int)glIsProgram(shaderSSAO.getProgramId()));

        RenderEngine::checkGlError("Test0");
        shaderSSAO.useProgram();
        RenderEngine::checkGlError("Test1");
        shaderSSAO.setInt("gPositionDepth", 0);
        shaderSSAO.setInt("gNormal", 1);
        shaderSSAO.setInt("texNoise", 2);

        RenderEngine::checkGlError("Test2");

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

            shaderSSAO.setVector3f(ShaderProgram::lazyArrayNames("kernelSamples[%i]", 64)[i],
                                   samp);
        }

        // Rand Noise (for TBN, de-banding)
//        BitmapImage texNoise{4, 4};
//        for (int i = 0; i < 4; ++i) {
//            for (int j = 0; j < 4; ++j) {
//                texNoise.setPixel(i, j, Colors::intRGBA(
//                        Colors::ofRGB(
//                                rand(gen) * 2.0f - 1.0f,
//                                rand(gen) * 2.0f - 1.0f,
//                                0.0f))
//                        );
//            }
//        }
//        m_TexNoise = Loader::loadTexture(texNoise);

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

    static void renderSSAO(Texture* gPositionDepth, Texture* gNormal)
    {
        auto _ap = fboSSAO->bindFramebuffer_ap();

        glClear(GL_COLOR_BUFFER_BIT);

        gPositionDepth->bindTexture2D(0);
        gNormal       ->bindTexture2D(1);
        m_TexNoise    ->bindTexture2D(2);


        shaderSSAO.useProgram();

        float fbWidth = Gui::maxWidth(), fbHeight = Gui::maxHeight();  // ! is not framebuffer size.
        shaderSSAO.setVector2f("noiseScale", glm::vec2(fbWidth / 4.0f, fbHeight / 4.0f));

        shaderSSAO.setViewProjection();

        ComposeRenderer::_DrawScreenQuad();

    }

};

#endif //ETHERTIA_SSAORENDERER_H
