#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 9) out;

in vec3 VertNorm[3];

out vec4 GeoColor;

const vec4 NormColor = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 BorderColor = vec4(0.2);

void pushNormLine(int i) {
    vec4 p = gl_in[i].gl_Position;

    gl_Position = p;
    GeoColor = NormColor;
    EmitVertex();

    gl_Position = p + vec4(VertNorm[i] * 0.2, 0.0);
    GeoColor = NormColor;
    EmitVertex();

    EndPrimitive();
}

void main() {

    // Normals
    pushNormLine(0);
    pushNormLine(1);
    pushNormLine(2);

    // Borders
    gl_Position = gl_in[0].gl_Position;
    GeoColor = BorderColor;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    GeoColor = BorderColor;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    GeoColor = BorderColor;
    EmitVertex();
    EndPrimitive();
}