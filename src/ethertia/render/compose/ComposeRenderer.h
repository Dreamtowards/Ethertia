//
// Created by Dreamtowards on 2023/2/12.
//

#ifndef ETHERTIA_COMPOSERENDERER_H
#define ETHERTIA_COMPOSERENDERER_H

// Deferred Renderer compose stage.
// take certain input: gPositionDepth, gNormal, gAlbedoSpec

class ComposeRenderer
{
public:

    ShaderProgram shaderCompose = Loader::loadShaderProgram("shaders/chunk/compose.{}");

    ComposeRenderer()
    {

        shaderCompose.useProgram();
        shaderCompose.setInt("gPositionDepth", 0);
        shaderCompose.setInt("gNormal", 1);
        shaderCompose.setInt("gAlbedoRoughness", 2);
        shaderCompose.setInt("gAmbientOcclusion", 3);

        // shaderCompose.setInt("panoramaMap", 5);
    }

    const char** UNIFORM_LIGHT_POSITION    = ShaderProgram::_GenArrayNames("lights[%i].position", 64);
    const char** UNIFORM_LIGHT_COLOR       = ShaderProgram::_GenArrayNames("lights[%i].color", 64);
    const char** UNIFORM_LIGHT_ATTENUATION = ShaderProgram::_GenArrayNames("lights[%i].attenuation", 64);
    const char** UNIFORM_LIGHT_DIRECTION   = ShaderProgram::_GenArrayNames("lights[%i].direction", 64);
    const char** UNIFORM_LIGHT_CONEANGLE   = ShaderProgram::_GenArrayNames("lights[%i].coneAngle", 64);


    void renderCompose(Texture* gPositionDepth, Texture* gNormal, Texture* gAlbedoRoughness, Texture* gAmbientOcclusion,
                       const std::vector<Light*>& lights)
    {
        gPositionDepth   ->bindTexture2D(0);
        gNormal          ->bindTexture2D(1);
        gAlbedoRoughness ->bindTexture2D(2);
        gAmbientOcclusion->bindTexture2D(3);


        // g_PanoramaTex = Loader::loadTexture(Loader::loadPNG(Loader::loadAssets("misc/skybox/hdri5.png")));

        shaderCompose.useProgram();

        shaderCompose.setVector3f("CameraPos", Ethertia::getCamera()->actual_pos);
        shaderCompose.setVector3f("CameraDir", Ethertia::getCamera()->direction);

        shaderCompose.setVector3f("cursorPos", Ethertia::getBrushCursor().position);
        shaderCompose.setFloat("cursorSize", Ethertia::getBrushCursor().brushSize);

//        shaderCompose.setFloat("fogGradient", fogGradient);
//        shaderCompose.setFloat("fogDensity", fogDensity);
//
//        shaderCompose.setFloat("debugVar0", debugVar0);
        shaderCompose.setFloat("debugVar1", EntityRenderer::debugVar1);
//        shaderCompose.setFloat("debugVar2", debugVar2);

        shaderCompose.setFloat("Time", Ethertia::getPreciseTime());
        shaderCompose.setFloat("DayTime", Ethertia::getWorld()->m_DayTime);

        shaderCompose.setMatrix4f("matInvView", glm::inverse(RenderEngine::matView));
        shaderCompose.setMatrix4f("matInvProjection", glm::inverse(RenderEngine::matProjection));


        shaderCompose.setInt("lightsCount", lights.size());
        for (int i = 0; i < std::min(64, (int)lights.size()); ++i)
        {
            Light* light = lights[i];
            shaderCompose.setVector3f(UNIFORM_LIGHT_POSITION[i], light->position);
            shaderCompose.setVector3f(UNIFORM_LIGHT_COLOR[i], light->color);
            shaderCompose.setVector3f(UNIFORM_LIGHT_ATTENUATION[i], light->attenuation);
            shaderCompose.setVector3f(UNIFORM_LIGHT_DIRECTION[i], light->direction);
            shaderCompose.setVector2f(UNIFORM_LIGHT_CONEANGLE[i], glm::vec2(light->coneAngle, 0));
        }



        _DrawScreenQuad();
    }

    static Model* _Model_ScreenQuad() {
        static Model* INST = nullptr;
        if (!INST) {
            // init RECT. def full viewport.
            float _RECT_POS[] = {1,-1, 1,1, -1,-1, -1,1};
            float _RECT_UV[]  = {1,0,  1,1, 0,0,  0,1};
            INST = Loader::loadModel(4, {
                    {2, _RECT_POS},
                    {2, _RECT_UV}
            });
        }
        return INST;
    }

    static void _DrawScreenQuad() {
        Model* m = _Model_ScreenQuad();
        glBindVertexArray(m->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m->vertexCount);
    }
};

#endif //ETHERTIA_COMPOSERENDERER_H
