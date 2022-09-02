//
// Created by Dreamtowards on 2022/5/24.
//

#ifndef ETHERTIA_SKYGRADIENTRENDERER_H
#define ETHERTIA_SKYGRADIENTRENDERER_H

#include <ethertia/client/render/shader/ShaderProgram.h>
#include <ethertia/client/Loader.h>
#include <ethertia/client/render/Model.h>
#include <ethertia/client/render/Camera.h>
#include <ethertia/client/render/RenderEngine.h>

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

        shader.setVector3f("CameraPos", Ethertia::getCamera()->position);
        // CamPos+

        shader.setMatrix4f("matModel", Mth::matModel(Ethertia::getCamera()->position + glm::vec3{0, 80, 0},glm::vec3 {500}));
        shader.setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
        shader.setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);


        glBindVertexArray(M_PLANE->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, M_PLANE->vertexCount);
    }
};

#endif //ETHERTIA_SKYGRADIENTRENDERER_H
