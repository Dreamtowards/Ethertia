#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex;  // not actual TexCoord., might have magic number (e.g. MtlId)
layout(location = 2) in vec3 in_norm;

layout(location = 0) out struct VS_Out
{
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 WorldNorm;
    vec3 BaryCoord;
    float MtlId;
} vs_out;

layout(set = 0, binding = 0) uniform UniformBuffer_T {
    mat4 matProjection;
    mat4 matView;
} ubo;

layout(push_constant) uniform PushConstant_T {
    mat4 matModel;
} pushconstant;


void main()
{
    mat4 matModel = pushconstant.matModel;

    vec4 worldpos = matModel * vec4(in_pos, 1);
    gl_Position = ubo.matProjection * ubo.matView * worldpos;


    vs_out.WorldPos = worldpos.xyz;
    vs_out.TexCoord = in_tex;
    vs_out.WorldNorm = normalize(mat3(matModel) * in_norm);

    // Barycentric Coordinate of the triangle, for material blend.
    int prim_vi = gl_VertexIndex % 3;
    vs_out.BaryCoord = vec3(prim_vi==0, prim_vi==1, prim_vi==2);

    int  MtlId   = int(floor(in_tex.x));
    bool PureMTL =     floor(in_tex.y) == -1;
    vs_out.MtlId = MtlId + (PureMTL ? 0.5 : 0);

}