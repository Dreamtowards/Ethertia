#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 TheColor;

void main()
{
    FragColor = vec4(TheColor, 1);
}
