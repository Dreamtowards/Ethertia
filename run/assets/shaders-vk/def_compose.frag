#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

//layout(set = 0, binding = 1) uniform UBO_Frag {
//    mat4 matModel;
//    mat4 matView;
//    mat4 matProjection;
//} ubo;

layout(set = 0, binding = 2) uniform sampler2D gPosition;
layout(set = 0, binding = 3) uniform sampler2D gNormal;
layout(set = 0, binding = 4) uniform sampler2D gAlbdeo;

void main()
{

    FragColor.rgb = texture(gAlbdeo, TexCoord).rgb;
}