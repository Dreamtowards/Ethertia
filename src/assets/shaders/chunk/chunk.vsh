#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_norm;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

out vec3 Norm;
flat out vec2 TexCoord;
out vec3 FragPos;

void main()
{
    vec4 worldpos = matModel * vec4(in_pos, 1.0);
    gl_Position = matProjection * matView * worldpos;

    Norm = normalize(vec3(matModel * vec4(in_norm, 0.0f)));
    TexCoord = in_texCoord;
    FragPos = worldpos.xyz;
}