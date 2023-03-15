//
// Created by Dreamtowards on 2023/2/13.
//

#ifndef ETHERTIA_DEBUGRENDERER_H
#define ETHERTIA_DEBUGRENDERER_H

class DebugRenderer
{
public:
    static DebugRenderer& Inst() {
        static DebugRenderer _INST;
        return _INST;
    }

    DECL_SHADER(SHADER_DEBUG_NORM, "shaders/debug/norm.{}");

    // limitLen+Pos: Only show partial (sphere) at pos with radius len
    void renderDebugGeo(VertexArrays* vao, glm::vec3 pos, glm::mat3 rot, float limitLen = 0, glm::vec3 limitPos = {}) {
        ShaderProgram& shader = *SHADER_DEBUG_NORM;

        shader.useProgram();

        shader.setMatrix4f("matModel", Mth::matModel(pos, rot, glm::vec3(1.0f)));
        shader.setViewProjection();

        if (limitLen) {
            shader.setFloat("limitLen", limitLen);
            shader.setVector3f("limitPos", limitPos);
        }

        RenderCommand::DrawArrays(vao);

        if (limitLen) {
            shader.setFloat("limitLen", 0);
        }
    }


    DECL_SHADER(SHADER_DEBUG_MODEL, "shaders/debug/model.{}");


    void drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat = true, bool boxOutline = false) {
        static float M_LINE[]  = {0,0,0,1,1,1};
        static VertexArrays* mLine = Loader::loadModel(2, {{3, M_LINE}});
        static float M_BOX[]  = {0,0,0,1,0,0, 1,0,0,1,0,1, 1,0,1,0,0,1, 0,0,1,0,0,0,
                                 0,1,0,1,1,0, 1,1,0,1,1,1, 1,1,1,0,1,1, 0,1,1,0,1,0,
                                 0,0,0,0,1,0, 1,0,0,1,1,0, 1,0,1,1,1,1, 0,0,1,0,1,1,};
        static VertexArrays* mBox = Loader::loadModel(24, {{3, M_BOX}});
        VertexArrays* model = boxOutline ? mBox : mLine;

        auto& shader = *SHADER_DEBUG_MODEL;
        shader.useProgram();

        shader.setViewProjection(viewMat);

        shader.setVector4f("color", color);
        shader.setVector3f("direction", dir);
        shader.setVector3f("position", pos);

        glBindVertexArray(model->vaoId);
        glDrawArrays(GL_LINES, 0, model->vertexCount);
    }

    void renderDebugWorldBasis() {
        int n = 4;
        int ex = 1;
        for (int x = -n; x <= n; ++x) {
            drawLine(glm::vec3(x, 0, -n-ex), glm::vec3(0, 0, 2*n+2*ex), Colors::BLACK30, true);
        }
        for (int z = -n; z <= n; ++z) {
            drawLine(glm::vec3(-n-ex, 0, z), glm::vec3(2*n+2*ex, 0, 0), Colors::BLACK30, true);
        }

        float len = 8.0f;
        drawLine(glm::vec3(0), glm::vec3(len, 0, 0), Colors::R, true);
        drawLine(glm::vec3(0), glm::vec3(0, len, 0), Colors::G, true);
        drawLine(glm::vec3(0), glm::vec3(0, 0, len), Colors::B, true);

    }
    void renderDebugBasis() {
        glm::vec3 p = glm::vec3(0.0f, 0.0f, -1.0f);
        float len = 0.2f;

        glm::mat3 matViewBasis = glm::mat3(Ethertia::getCamera().matView);
        drawLine(p, matViewBasis* glm::vec3(len, 0, 0), Colors::R, false);
        drawLine(p, matViewBasis * glm::vec3(0, len, 0), Colors::G, false);
        drawLine(p, matViewBasis * glm::vec3(0, 0, len), Colors::B, false);
    }



};

#endif //ETHERTIA_DEBUGRENDERER_H
