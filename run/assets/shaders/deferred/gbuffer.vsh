#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texCoord;  // if use MtlId insteaf of UV, x=MtlId, y=const 1342 (magic number)
layout (location = 2) in vec3 in_norm;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform float Time;
uniform float WaveFactor;

out Vsh {
    vec3 FragPos;
    vec3 Norm;
    vec2 TexCoord;
    float MtlId;
} vsh;

void main()
{
    vec4 worldpos = matModel * vec4(in_pos, 1.0);

    // Folige Waving
    worldpos.xyz += vec3(cos(worldpos.yzx * 1000 + Time * 1.3).xyz) * WaveFactor;

    gl_Position = matProjection * matView * worldpos;

    vsh.Norm = normalize(vec3(matModel * vec4(in_norm, 0.0f)));
    vsh.TexCoord = in_texCoord;
    vsh.FragPos = worldpos.xyz;
    vsh.MtlId = in_texCoord.x;  // MtlId cant coexist with UV. since UV are structure necessary, MtlId are hidden into UV. (if have Mtl)
}