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

    DECL_SHADER(SHADER_DEBUG_NORM, ShaderProgram::decl("shaders/debug/norm.{}", true));

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


//    DECL_SHADER(SHADER_DEBUG_MODEL, ShaderProgram::decl("shaders/debug/model.{}"));


};

#endif //ETHERTIA_DEBUGRENDERER_H
