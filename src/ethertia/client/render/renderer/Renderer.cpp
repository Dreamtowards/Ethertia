//
// Created by Dreamtowards on 2022/5/24.
//


#include "Renderer.h"
#include <ethertia/client/Ethertia.h>
#include <ethertia/client/render/RenderEngine.h>
#include <ethertia/client/render/Camera.h>

void Renderer::setShaderMVP(ShaderProgram* shader, glm::mat4 matModel) {

    shader->setMatrix4f("matModel", matModel);
    shader->setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
    shader->setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);

}

void Renderer::setShaderCamPos(ShaderProgram* shader) {

    shader->setVector3f("CameraPos", Ethertia::getCamera()->position);
}