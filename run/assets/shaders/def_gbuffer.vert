#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;  // MtlId instead of TexCoord.
layout(location = 2) in vec3 in_norm;

layout(location = 0) out struct VS_Out
{
    vec3 WorldPos;
    vec3 WorldNorm;
    vec3 BaryCoord;
    vec3 MtlIds;
} vs_out;

layout(set = 0, binding = 0) uniform UniformBufferVert_T {
    mat4 matProjection;
    mat4 matView;
} ubo;

layout(push_constant) uniform PushConstant_T {
    mat4 matModel;
} pc;


void main()
{
    mat4 matModel = pc.matModel;

    vec4 WorldPos = matModel * vec4(in_pos, 1);
    gl_Position = ubo.matProjection * ubo.matView * WorldPos;

    
    vs_out.WorldPos = WorldPos.xyz;
    vs_out.WorldNorm = normalize(mat3(matModel) * in_norm);

    int  TriId = gl_PrimitiveID;
    vec3 TriIdCol = vec3(
        fract(cos(TriId) * 43758.6453),
        fract(cos(TriId) * 78456.9932),
        fract(cos(TriId) * 29034.8291)
    );

    // Barycentric Coordinate of the triangle, for material blend.
    int  vi = gl_VertexIndex % 3;
    vec3 vm = vec3(vi==0, vi==1, vi==2);
    vs_out.BaryCoord = TriIdCol;

    float MtlId   = floor(in_uv.x);
    vs_out.MtlIds = vm * MtlId;
}