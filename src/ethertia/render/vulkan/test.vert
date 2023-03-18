#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_col;

layout(location = 0) out vec3 SomeColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 0, 1);

    SomeColor = in_col;
}