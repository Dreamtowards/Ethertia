#version 330 core
layout (location = 0) in vec3 in_pos;

out vec3 TexCoord;

uniform mat4 matProjection;
uniform mat4 matView;

void main() {

    TexCoord = in_pos;

    vec4 p = matProjection * mat4(mat3(matView)) * vec4(in_pos, 1.0);
    gl_Position = p.xyww;  // make clip-space z equals 1.0. (z / w == w / w == 1.0)
}