//
// Created by Dreamtowards on 2022/5/24.
//

#ifndef ETHERTIA_SKYGRADIENTRENDERER_H
#define ETHERTIA_SKYGRADIENTRENDERER_H

#include <ethertia/render/ShaderProgram.h>
#include <ethertia/render/Model.h>
#include <ethertia/render/Camera.h>
#include <ethertia/render/RenderEngine.h>
#include <ethertia/util/Loader.h>

class SkyGradientRenderer
{
    ShaderProgram shader = Loader::loadShaderProgram("shaders/sky/gradient.{}");

    inline static Model* M_PLANE;

public:
    SkyGradientRenderer() {

        float vts[] = {
             1, 0, -1,   1, 0, 1,   -1, 0, 1,  // Bottom -Y
            -1, 0, -1,   1, 0, -1,  -1, 0, 1,
        };
        M_PLANE = Loader::loadModel(6, {{3, vts}});
    }

    void render() {

        shader.useProgram();

        shader.setVector3f("CameraPos", Ethertia::getCamera()->position);
        // CamPos+


        shader.setMVP(
                Mth::matModel(/*Ethertia::getCamera()->position + */glm::vec3{0, 80, 0},glm::vec3 {500})
        );


        glBindVertexArray(M_PLANE->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, M_PLANE->vertexCount);
    }
};

#endif //ETHERTIA_SKYGRADIENTRENDERER_H
