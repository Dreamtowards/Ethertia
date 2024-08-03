//
// Created by Dreamtowards on 2023/3/13.
//

#include "RenderCommand.h"

#include <stack>
#include <glad/glad.h>



void RenderCommand::Clear(glm::vec4 c)
{
    glClearColor(c.x, c.y, c.z, c.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int RenderCommand::CheckError(std::string_view phase) {
    GLuint err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        Log::warn("###### GL Error @{} ######", phase);
        Log::warn("ERR: {}", err);
        return err;
    }
    return 0;
}

void RenderCommand::DrawArrays(VertexArrays* vao)
{
    assert(vao->vboId);
    glBindVertexArray(vao->vaoId);
    glDrawArrays(GL_TRIANGLES, 0, vao->vertexCount);
}

void RenderCommand::DrawFullQuad() {
    static VertexArrays* _Quad = nullptr;
    if (!_Quad) {
        // init RECT. def full viewport.
        // vec2 pos, vec2 uv;
        float _VTX[] = {
                1,-1,    1,0,
                1,1,     1,1,
                -1,-1,   0,0,
                -1,1,    0,1
        };
        _Quad = Loader::loadVertexBuffer(4, {2, 2}, _VTX);
    }
    glBindVertexArray(_Quad->vaoId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}




//////////////// Texture ////////////////

Texture* Texture::GenTexture(int w, int h, int t)
{
    Texture* tex = new Texture();
    tex->width = w;
    tex->height = h;
    tex->target = t;
    glGenTextures(1, (GLuint*)&tex->texId);
    return tex;
}

Texture::~Texture() {
    glDeleteTextures(1, (GLuint*)&texId);
}

void Texture::BindTexture(int slot) {
    assert(target != 0);

    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(target, texId);
}

BitmapImage* Texture::GetTexImage()
{
    void* pixels = new char[width * height * 4];
    glBindTexture(GL_TEXTURE_2D, texId);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return new BitmapImage(width, height, (unsigned int*)pixels);
}












//////////////// Framebuffer ////////////////

