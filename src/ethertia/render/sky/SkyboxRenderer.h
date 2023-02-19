//
// Created by Dreamtowards on 2022/10/4.
//

#ifndef ETHERTIA_SKYBOXRENDERER_H
#define ETHERTIA_SKYBOXRENDERER_H

class SkyboxRenderer
{
    ShaderProgram m_Shader = Loader::loadShaderProgram("shaders/sky/skybox.{}");

    inline static Model* m_SkyboxModel = nullptr;

public:

    SkyboxRenderer() {


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
        m_SkyboxModel = Loader::loadModel(36, {
                {3, skyboxVertices}
        });
    }

    void render(Texture* cubemap, glm::vec3 rotAxis, float angle, glm::vec4 colMul = {1,1,1,1}) {

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        m_Shader.useProgram();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texId);

        m_Shader.setMVP(Mth::rot(glm::normalize(rotAxis), angle));

        m_Shader.setVector4f("ColorMul", colMul);

        glBindVertexArray(m_SkyboxModel->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, m_SkyboxModel->vertexCount);

        glDepthFunc(GL_LESS); // set depth function back to default
        glDepthMask(GL_TRUE);
    }


    void renderWorldSkybox(float daytime) {
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
