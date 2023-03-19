#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 TheColor;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
//    FragColor = vec4(TexCoord, 0, 1);
    FragColor = vec4(TheColor * texture(texSampler, TexCoord).rgb, 1.0);
}
