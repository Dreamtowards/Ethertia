#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex;
layout(location = 2) in vec3 in_norm;

layout(location = 0) out vec3 WorldPos;
layout(location = 1) out vec2 TexCoord;
layout(location = 2) out vec3 WorldNorm;

layout(set = 0, binding = 0) uniform UBO_T {
    mat4 matProjection;
    mat4 matView;
    // mat4 matModel;
} ubo;

//layout(push_constant) uniform PushConstant_T {
//    mat4 matModel;
//} pushconstant;


void main()
{
    mat4 matModel = mat4(1); // pushconstant.matModel

    vec4 worldpos = matModel * vec4(in_pos, 1);
    gl_Position = ubo.matProjection * ubo.matView * worldpos;


    WorldPos = worldpos.xyz;
    TexCoord = in_tex;
    WorldNorm = normalize(mat3(matModel) * in_norm);
}