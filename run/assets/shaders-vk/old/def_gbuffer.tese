#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(triangles, equal_spacing, cw) in;  // our VertexData is CCW-FrontFace, CullBack. however there need to be set 'cw' for Correct. why.

layout(location = 0) in struct TC_Out
{
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 WorldNorm;
    float MtlId;
} te_in[];

layout(location = 0) out struct TE_Out
{
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 WorldNorm;
    vec3 BaryCoord;
    vec3 MtlIds;
} te_out;

void main()
{
#define BaryLerp(arr, ATTR) (gl_TessCoord.x * arr[0].ATTR + gl_TessCoord.y * arr[1].ATTR + gl_TessCoord.z * arr[2].ATTR)

    te_out.WorldPos  = BaryLerp(te_in, WorldPos);
    te_out.TexCoord  = BaryLerp(te_in, TexCoord);
    te_out.WorldNorm = BaryLerp(te_in, WorldNorm);
    te_out.BaryCoord = gl_TessCoord;
    te_out.MtlIds = vec3(te_in[0].MtlId, te_in[1].MtlId, te_in[2].MtlId);

    gl_Position = BaryLerp(gl_in, gl_Position);
}