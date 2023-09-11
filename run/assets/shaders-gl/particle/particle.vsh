#version 330 core
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex;

out vec2 TexCoord;

uniform mat4 matView;
uniform mat4 matProjection;

uniform vec3 position;
uniform float size;

void main()
{

    mat4 matPV = matProjection * matView;

    vec3 CamX = vec3(matView[0][0], matView[1][0], matView[2][0]);
    vec3 CamY = vec3(matView[0][1], matView[1][1], matView[2][1]);

    vec3 worldpos = position +
                    CamX * in_pos.x * size +
                    CamY * in_pos.y * size;
                    // Vertical: vec3(0,1,0) * in_pos.y;

    gl_Position = matPV * vec4(worldpos, 1.0);

    TexCoord = in_tex;
}