//
// Created by Dreamtowards on 2022/11/14.
//

#ifndef ETHERTIA_FRAMEBUFFER_H
#define ETHERTIA_FRAMEBUFFER_H

#include <ethertia/render/Texture.h>
#include <ethertia/util/Loader.h>

#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>


class Framebuffer
{
private:
    Framebuffer() = default;  // should explicitly create FBO. use Framebuffer::glfGenFramebuffer();
public:
    int fboId  = 0;
    int width  = 0;
    int height = 0;

    // Texture Attachments.
    Texture* texColor[8]     = {};
    Texture* texDepth        = nullptr;
    Texture* texDepthStencil = nullptr;  // RBO?

    static Framebuffer* GenFramebuffer(int w, int h, const std::function<void(Framebuffer*)>& initfunc);
    ~Framebuffer();


    void attachColorTexture(int i, int intlfmt = GL_RGB, int fmt = GL_RGB, int type = GL_UNSIGNED_BYTE) {
        texColor[i] = internalAttachTexture2D(GL_COLOR_ATTACHMENT0+i, intlfmt, fmt, type);
    }
    void attachDepthTexture() {
        texDepth = internalAttachTexture2D(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    }
    void attachDepthStencilTexture() {
        texDepthStencil = internalAttachTexture2D(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    }
    Texture* internalAttachTexture2D(int attachment, int intlfmt, int fmt, int type) {
        auto* tex = Loader::loadTexture(width, height, nullptr, intlfmt, fmt, type);

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->texId, 0);
        return tex;
    }


    void attachDepthStencilRenderbuffer() {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    static void BeginFramebuffer(Framebuffer* fbo);
    static void EndFramebuffer();

    // auto pop
    [[nodiscard]]
    DtorCaller BeginFramebuffer_Scoped() {
        Framebuffer::BeginFramebuffer(this);

        return DtorCaller{[](){
            Framebuffer::EndFramebuffer();
        }};
    }




    void checkFramebufferStatus() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log::warn("ERR: Framebuffer is not complete");
        }
    }

    // disableColorBuffers();  use for e.g. ShadowMapping render that only need depth texture.
    void disableDrawBuffers() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // tell GL render to which Color Attachments/Textures
    void setupMRT(std::initializer_list<int> l) {
        size_t n = l.size();
        std::vector<GLuint> arr(n);
        Collections::range(arr.data(), n, (GLuint)GL_COLOR_ATTACHMENT0);
        glDrawBuffers(n, arr.data());
    }

    // resize() ?

};


#endif //ETHERTIA_FRAMEBUFFER_H
