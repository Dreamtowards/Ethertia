//
// Created by Dreamtowards on 2023/3/13.
//

#include "RenderCommand.h"

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
    vao->BindVertexArrays();
    glDrawArrays(GL_TRIANGLES, 0, vao->vertexCount);
}

void RenderCommand::DrawFullQuad() {
    static VertexArrays* _Quad = nullptr;
    if (!_Quad) {
        // init RECT. def full viewport.
        float _RECT_POS[] = {1,-1, 1,1, -1,-1, -1,1};
        float _RECT_UV[]  = {1,0,  1,1, 0,0,  0,1};
        _Quad = Loader::loadModel(4, {
                {2, _RECT_POS},
                {2, _RECT_UV}
        });
    }
    _Quad->BindVertexArrays();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



//////////////// VAO, VertexArray ////////////////

VertexArrays* VertexArrays::GenVertexArrays() {
    VertexArrays* vao = new VertexArrays();
    glGenVertexArrays(1, (GLuint*)&vao->vaoId);
    return vao;
}

VertexArrays::~VertexArrays() {
    glDeleteVertexArrays(1, (GLuint*)&vaoId);
}

void VertexArrays::BindVertexArrays() {
    glBindVertexArray(vaoId);
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

Framebuffer* Framebuffer::GenFramebuffer(int w, int h, const std::function<void(Framebuffer*)> &initfunc)
{
    Framebuffer* fbo = new Framebuffer();
    fbo->width = w;
    fbo->height = h;
    glGenFramebuffers(1, (GLuint*)&fbo->fboId);

    Framebuffer::BeginFramebuffer(fbo);

    initfunc(fbo);

    fbo->checkFramebufferStatus();

    Framebuffer::EndFramebuffer();
    return fbo;
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, (GLuint*)&fboId);
}


static void BindFramebuffer(int fboId, int w, int h) {
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glViewport(0, 0, w, h);
}

static void BindFramebuffer(Framebuffer* fbo) {
    BindFramebuffer(fbo->fboId, fbo->width, fbo->height);
}

inline static std::stack<Framebuffer*> g_FBO_Stack;

void Framebuffer::BeginFramebuffer(Framebuffer* fbo)
{
    BindFramebuffer(fbo);
    g_FBO_Stack.push(fbo);
}

void Framebuffer::EndFramebuffer()
{
    g_FBO_Stack.pop();

    if (g_FBO_Stack.empty()) {  // bind Main Framebuffer
        auto& win = Ethertia::getWindow();
        BindFramebuffer(0, win.getFramebufferWidth(), win.getFramebufferHeight());
    } else {
        BindFramebuffer(g_FBO_Stack.top());
    }
}



