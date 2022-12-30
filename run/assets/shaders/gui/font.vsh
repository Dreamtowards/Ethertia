#version 330 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_texCoord;

out vec2 TexCoord;

uniform float glyphWidths[256];  // almost static.

uniform int chars[128];  // batch rendering, once max 128 chars.
uniform vec2 offset[128];
uniform vec2 scale;  // ch std_size

void main() {
    int ch = chars[gl_InstanceID];
    float glyph_w = glyphWidths[ch];

    gl_Position = vec4(in_pos * vec2(glyph_w*scale.x, scale.y) + offset[gl_InstanceID], 0.0, 1.0);

    int row = ch / 16;
    int col = ch % 16;
    vec2 texpos = vec2(col/16.0, (15.0/16.0) - row/16.0);
    TexCoord = in_texCoord * vec2(1.0/16.0 * glyph_w, 1.0/16.0) + texpos;
}