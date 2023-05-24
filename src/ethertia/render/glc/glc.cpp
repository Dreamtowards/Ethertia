//
// Created by Dreamtowards on 2023/5/23.
//


#include "glc.h"

#include <glad/glad.h>

void test()
{

}

Texture* glc::CreateTexture(GLenum target)
{
    glc::Texture* tex = new glc::Texture();

    glCreateTextures(GL_TEXTURE_2D, 1, &tex->textureId);
}

void glc::TextureParameter(Texture* tex, GLenum pname, GLenum param)
{
    glTextureParameteri(tex->textureId, pname, param);
}


void glc::BindTextureUnit(int unit, Texture *tex)
{
    glBindTextureUnit(unit, tex->textureId);
}
