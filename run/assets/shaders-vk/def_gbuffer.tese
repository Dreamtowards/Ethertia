#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(triangles, equal_spacing, ccw) in;

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
    // Since TessEval's layout=triangles -> BaryCoord; (layout=quads -> UV.)

#define bary_lerp(arr, ATTR) gl_TessCoord.x * arr[0].ATTR + gl_TessCoord.y * arr[1].ATTR + gl_TessCoord.z * arr[2].ATTR

    te_out.WorldPos  = bary_lerp(te_in, WorldPos);
    te_out.TexCoord  = bary_lerp(te_in, TexCoord);
    te_out.WorldNorm = bary_lerp(te_in, WorldNorm);
    te_out.BaryCoord = gl_TessCoord;
    te_out.MtlIds = vec3(te_in[0].MtlId, te_in[1].MtlId, te_in[2].MtlId);

    gl_Position = bary_lerp(gl_in, gl_Position);
}