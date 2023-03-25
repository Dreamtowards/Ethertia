#version 330 core
layout(location = 0) in vec3 in_pos;

uniform mat4 matShadowSpace;
uniform mat4 matModel;


void main()
{

    gl_Position = matShadowSpace * matModel * vec4(in_pos, 1.0);


    // Fish Eye, Dome Projection
    const float SHADOW_MAP_BIAS = 0.8;  // same of compose.fsh, shadow.fsh
    float distort_f = (1.0 - SHADOW_MAP_BIAS) + length(gl_Position.xy) * SHADOW_MAP_BIAS;
    gl_Position.xy /= distort_f;
}