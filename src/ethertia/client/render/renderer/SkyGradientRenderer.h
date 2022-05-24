//
// Created by Dreamtowards on 2022/5/24.
//

#ifndef ETHERTIA_SKYGRADIENTRENDERER_H
#define ETHERTIA_SKYGRADIENTRENDERER_H

#include <ethertia/client/render/shader/ShaderProgram.h>
#include <ethertia/client/Loader.h>
#include <ethertia/client/render/Model.h>
#include <ethertia/util/Eth.h>
#include <ethertia/client/render/Camera.h>

class SkyGradientRenderer
{
    ShaderProgram shader{Loader::loadAssetsStr("shaders/sky/gradient.vsh"),
                         Loader::loadAssetsStr("shaders/sky/gradient.fsh")};

    inline static Model* M_PLANE;

public:
    SkyGradientRenderer() {

        float vts[] = {
            -1, 0, 1, 1, 0, 1, 1, 0, -1,  // Bottom +Y
            -1, 0, 1, 1, 0, -1, -1, 0, -1
        };
        M_PLANE = Loader::loadModel(6, {{3, vts}});
    }

    void render() {

        shader.useProgram();

        Renderer::setShaderCamPos(&shader);
        // CamPos+
        Renderer::setShaderMVP(&shader, Mth::matModel(
                Eth::getCamera()->position + glm::vec3{0, -100, 0},
                glm::vec3 {500}));

        glBindVertexArray(M_PLANE->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, M_PLANE->vertexCount);
    }
};

#endif //ETHERTIA_SKYGRADIENTRENDERER_H
