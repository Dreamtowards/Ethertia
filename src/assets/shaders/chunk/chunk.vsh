#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_norm;
layout (location = 3) in float in_mtl;  // material id. integer.

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;


out Vsh {
    vec3 FragPos;
    vec3 Norm;
    vec2 TexCoord;
    float MtlId;
} vsh;

void main()
{
    vec4 worldpos = matModel * vec4(in_pos, 1.0);
    gl_Position = matProjection * matView * worldpos;

    vsh.Norm = normalize(vec3(matModel * vec4(in_norm, 0.0f)));
    vsh.TexCoord = in_texCoord;
    vsh.FragPos = worldpos.xyz;
    vsh.MtlId = in_mtl;
}