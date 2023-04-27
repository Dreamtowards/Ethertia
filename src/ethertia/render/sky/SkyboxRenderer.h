//
// Created by Dreamtowards on 2022/10/4.
//

#ifndef ETHERTIA_SKYBOXRENDERER_H
#define ETHERTIA_SKYBOXRENDERER_H

class SkyboxRenderer
{
    DECL_SHADER(SHADER, ShaderProgram::decl("shaders/sky/skybox.{}"));

    inline static VertexArrays* vao_Skybox = nullptr;

public:

    static void init() {

        float skyboxVertices[] = {
                // positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };
        vao_Skybox = Loader::loadModel(36, {
                {3, skyboxVertices}
        });
    }

    static void render(Texture* cubemap, glm::vec3 rotAxis, float angle, glm::vec4 colMul = {1,1,1,1}) {

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        cubemap->BindTexture();

        SHADER->useProgram();
        SHADER->setMVP(Mth::rot(glm::normalize(rotAxis), angle));
        SHADER->setVector4f("ColorMul", colMul);

        RenderCommand::DrawArrays(vao_Skybox);

        glDepthFunc(GL_LESS); // set depth function back to default
        glDepthMask(GL_TRUE);
    }


    static void renderWorldSkybox(float daytime) {
        float time = Ethertia::getPreciseTime();

        if (daytime >= 0.25 && daytime < 0.75)  // 6AM-6PM
        {
//        Tex_NightStar = Loader::loadCubeMap1("misc/sky/starfield1.png");

            GlState::blendMode(GlState::ADD);
            static Texture* TEX_CLOUDS = Loader::loadCubeMap_3x2("misc/sky/cloud1.png");
            render(TEX_CLOUDS, {0,1,0}, -time / 60, glm::vec4{0.15});

//                GlState::blendMode(GlState::ALPHA);
//                m_SkyboxRenderer->render(SkyboxRenderer::Tex_Cloud, {0,1,0}, -time / 60, glm::vec4{1});

            GlState::blendMode(GlState::ADD);
            static Texture *Atmos = Loader::loadCubeMap_3x2("misc/sky/cloudbox/atmosphere.png");
            render(Atmos, {0,1,0}, time / 60);

            static Texture* Horizon = Loader::loadCubeMap_3x2("misc/sky/cloudbox/horizon.png");
            render(Horizon, {0,1,0}, -time/40);
        }
        else
        {
            GlState::blendMode(GlState::ADD);

            static Texture* Stars = Loader::loadCubeMap_3x2("misc/sky/nightbox/stars.png");
            render(Stars, {0,0,1}, time/60);

            static Texture *Aurora = Loader::loadCubeMap_3x2("misc/sky/nightbox/aurora.png");
            render(Aurora, {-0.3,1,0}, -time / 70);

            static Texture *Galaxies = Loader::loadCubeMap_3x2("misc/sky/nightbox/galaxies.png");
            render(Galaxies, {0.4,0.4,0}, -time / 70);

            static Texture *Depth = Loader::loadCubeMap_3x2("misc/sky/nightbox/depth.png");
            render(Depth, {1,0,0}, -time / 50);
        }

    }
};

#endif //ETHERTIA_SKYBOXRENDERER_H
