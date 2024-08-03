//
// Created by Dreamtowards on 2024/8/3.
//


#include "glx.h"
#include <stack>

#include "ethertia/util/Loader.h"
#include <ethertia/render/Window.h>


#pragma region Framebuffer

Framebuffer* Framebuffer::Create(int w, int h, const std::function<void(Framebuffer&)>& initfunc)
{
    auto* fbo = new Framebuffer();
    fbo->m_Width = w;
    fbo->m_Height = h;
    glGenFramebuffers(1, (GLuint*)&fbo->m_fboId);

    Framebuffer::BeginFramebuffer(fbo);
    initfunc(*fbo);
    fbo->CheckStatus();
    Framebuffer::EndFramebuffer();
    return fbo;
}


static void BindFramebuffer(int fboId, int w, int h) {
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glViewport(0, 0, w, h);
}
static void BindFramebuffer(Framebuffer* fbo) {
    BindFramebuffer(fbo->m_fboId, fbo->m_Width, fbo->m_Height);
}

inline static std::stack<Framebuffer*> g_FboStack;

void Framebuffer::BeginFramebuffer(Framebuffer* fbo) {
    BindFramebuffer(fbo);
    g_FboStack.push(fbo);
}

void Framebuffer::EndFramebuffer() {
    g_FboStack.pop();

    if (g_FboStack.empty()) {  // bind Main Framebuffer
        auto s = Window::FramebufferSize();
        BindFramebuffer(0, s.x, s.y);
    } else {
        BindFramebuffer(g_FboStack.top());
    }
}


Texture *Framebuffer::InternalAttachTexture2D(int attachment, int internalFormat, int format, int type) {
    Texture* tex = Loader::LoadTexture(m_Width, m_Height, nullptr, internalFormat, format, type);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->id(), 0);
    return tex;
}


#pragma endregion



//BitmapImage* Texture::GetTexImage()
//{
//    void* pixels = new char[width * height * 4];
//    glBindTexture(GL_TEXTURE_2D, texId);
//    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//    return new BitmapImage(width, height, (unsigned int*)pixels);
//}