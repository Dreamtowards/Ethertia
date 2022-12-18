//
// Created by Dreamtowards on 2022/12/16.
//

#ifndef ETHERTIA_WATERRENDERER_H
#define ETHERTIA_WATERRENDERER_H

#include <ethertia/util/Loader.h>
#include <ethertia/render/shader/ShaderProgram.h>

class WaterRenderer
{
public:

    ShaderProgram shader{Loader::loadAssetsStr("shaders/water/water.vsh"),
                         Loader::loadAssetsStr("shaders/water/water.fsh")};




    void render(Model* model) {

        shader.useProgram();

        glBindVertexArray(model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
    }


};

#endif //ETHERTIA_WATERRENDERER_H
