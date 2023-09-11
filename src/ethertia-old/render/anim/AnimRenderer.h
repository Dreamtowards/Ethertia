//
// Created by Dreamtowards on 2023/3/7.
//

#ifndef ETHERTIA_ANIMRENDERER_H
#define ETHERTIA_ANIMRENDERER_H

#include <ethertia/render/ShaderProgram.h>
#include <ethertia/util/Loader.h>

#include <ethertia/render/anim/Animation.h>

class AnimRenderer
{
public:

    DECL_SHADER(SHADER, ShaderProgram::decl("shaders/anim/anim.{}"));


    // Model VertexData Attribs
    // ...
    // vec3i jointIdxs;
    // vec3  jointWeights
    static void render(VertexArrays* vao, const std::vector<Animation::Joint>& joints)
    {
        SHADER->useProgram();

//        glm::mat4 joint_tranfs[50] = Animation::getJointTransfroms();
//
//        static auto uJointTrans = ShaderProgram::_GenArrayNames("JointTrans[{}]", 50);
//        for (int i = 0; i < joints.size(); ++i) {
//            shader.setMatrix4f(uJointTrans[i], joints[i].curr_trans);
//        }

        SHADER->setViewProjection();


        RenderCommand::DrawArrays(vao);

    }

};

#endif //ETHERTIA_ANIMRENDERER_H
