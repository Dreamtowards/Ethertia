//
// Created by Dreamtowards on 2023/5/12.
//

#ifndef ETHERTIA_GLC_H
#define ETHERTIA_GLC_H

typedef uint32_t GLenum;
typedef uint32_t GLuint;

namespace glc
{

    class Texture
    {
        GLuint textureId;
        GLuint target;

        int width = 0;
        int height = 0;
    };

    // glGenTextures() glBindTexture()
    Texture* CreateTexture(GLenum target);

    void TextureParameter(Texture* tex, GLenum pname, GLenum param);

    void BindTextureUnit(int unit, Texture* tex);




    class VertexArrays
    {
        GLuint vaoId;
        GLuint vboId;       // interleaved vertex data
        GLuint eboId = 0;   // optional, 0 if non indexed.
        uint32_t vertexCount = 0;
    };

    class ShaderProgram
    {
        GLuint programId;


    };


}

#endif //ETHERTIA_GLC_H
