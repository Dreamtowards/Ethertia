//
// Created by Dreamtowards on 2022/10/4.
//

#ifndef ETHERTIA_SKYBOXRENDERER_H
#define ETHERTIA_SKYBOXRENDERER_H

class SkyboxRenderer
{
    ShaderProgram m_Shader = Loader::loadShaderProgram("shaders/sky/skybox.{}");


    Texture* m_Cubemap = nullptr;
    Model* m_SkyboxModel = nullptr;

public:
    SkyboxRenderer() {

        m_Cubemap = Loader::loadCubeMap("misc/skybox/{}.jpg");

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

    void render() {

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        m_Shader.useProgram();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_Cubemap->texId);

        m_Shader.setViewProjection();

        glBindVertexArray(m_SkyboxModel->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, m_SkyboxModel->vertexCount);

        glDepthFunc(GL_LESS); // set depth function back to default
        glDepthMask(GL_TRUE);
    }

};

#endif //ETHERTIA_SKYBOXRENDERER_H
