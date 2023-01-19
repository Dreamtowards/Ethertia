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
    inline static Texture* Tex_Cloud = nullptr;
//    inline static Texture* Tex_NightStar = nullptr;

    SkyboxRenderer() {

        Tex_Cloud = Loader::loadCubeMap1("misc/sky/cloud1.png");
//        Tex_NightStar = Loader::loadCubeMap1("misc/sky/starfield1.png");

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

};

#endif //ETHERTIA_SKYBOXRENDERER_H
