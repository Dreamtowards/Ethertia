#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(vertices = 3) out;

struct VS_Out
{
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 WorldNorm;
    float MtlId;
};

layout(location = 0) in VS_Out tc_in[];

layout(location = 0) out VS_Out tc_out[];

void main()
{
    const int VertIdx = gl_InvocationID;  // VertIdx within this Patch.
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    tc_out[gl_InvocationID] = tc_in[gl_InvocationID];
//    vs_in.WorldPos_out[VertIdx]  = WorldPos_in[VertIdx];
//    vs_in.TexCoord_out[VertIdx]  = TexCoord_in[VertIdx];
//    vs_in.WorldNorm_out[VertIdx] = WorldNorm_in[VertIdx];
//    vs_in.MtlId_out[VertIdx]  = MtlId_in[VertIdx];

    if (VertIdx == 0)
    {
        gl_TessLevelOuter[0] = 1;
        gl_TessLevelOuter[1] = 1;
        gl_TessLevelOuter[2] = 1;
        gl_TessLevelOuter[3] = 1;

        gl_TessLevelInner[0] = 1;
        gl_TessLevelInner[1] = 1;
    }
}