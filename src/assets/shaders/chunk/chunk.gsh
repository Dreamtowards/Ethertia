#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in Vsh {
    vec3 FragPos;
    vec3 Norm;
    vec2 TexCoord;
    float MtlId;
} vsh[];

out vec3 Norm;
out vec3 FragPos;
out vec2 TexCoord;

flat out vec3 TriMtlId;
out vec3 TriMtlWeight;


void main() {

    vec3 _TriMtlId = vec3(vsh[0].MtlId, vsh[1].MtlId, vsh[2].MtlId);

    Norm     = vsh[0].Norm;
    FragPos  = vsh[0].FragPos;
    TexCoord = vsh[0].TexCoord;
    TriMtlId = _TriMtlId;
    TriMtlWeight = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    Norm     = vsh[1].Norm;
    FragPos  = vsh[1].FragPos;
    TexCoord = vsh[1].TexCoord;
    TriMtlId = _TriMtlId;
    TriMtlWeight = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    Norm     = vsh[2].Norm;
    FragPos  = vsh[2].FragPos;
    TexCoord = vsh[2].TexCoord;
    TriMtlId = _TriMtlId;
    TriMtlWeight = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}


