#version 330 core
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_norm;

layout(location = 3) in vec3 in_joint_ids;      // vec3i
layout(location = 4) in vec3 in_joint_weights;


uniform mat4 matProjection;
uniform mat4 matView;

uniform mat4 JointTransforms[50];

void main()
{

    mat4 JointT = JointTransforms[int(in_joint_ids[0])] * in_joint_weights[0];

    gl_Position = matProjection * matView * vec4(in_pos, 1.0);
}