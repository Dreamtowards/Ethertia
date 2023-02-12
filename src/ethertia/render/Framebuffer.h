//
// Created by Dreamtowards on 2022/11/14.
//

#ifndef ETHERTIA_FRAMEBUFFER_H
#define ETHERTIA_FRAMEBUFFER_H

#include <ethertia/render/Texture.h>
#include <ethertia/gui/Gui.h>
#include <ethertia/util/Loader.h>

class Framebuffer
{
private:
    Framebuffer() {}  // should explicitly create FBO. use Framebuffer::glfGenFramebuffer();
public:

    uint fboId  = 0;
    int width  = 0;
    int height = 0;

    // Texture Attachments.
    Texture* texColor[8]     = {};
    Texture* texDepth        = nullptr;
    Texture* texDepthStencil = nullptr;  // RBO?

    static Framebuffer* glfGenFramebuffer(int w, int h) {
        auto fbo = new Framebuffer();
        glGenFramebuffers(1, &fbo->fboId);

        fbo->width = w;
        fbo->height = h;
        return fbo;
    }

    ~Framebuffer() {

        glDeleteFramebuffers(1, &fboId);
    }


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
        uint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }





    static void doBindFramebuffer(int fboId, int w, int h) {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glViewport(0, 0, w, h);
    }
    static void doBindFramebuffer(Framebuffer* fbo) {
        Framebuffer::doBindFramebuffer(fbo->fboId, fbo->width, fbo->height);
    }

    inline static std::stack<Framebuffer*> globalFboStack;

    static void gPushFramebuffer(Framebuffer* fbo) {
        Framebuffer::doBindFramebuffer(fbo);

        globalFboStack.push(fbo);
    }

    static void gPopFramebuffer() {
        globalFboStack.pop();

        if (globalFboStack.empty()) {  // bind Main Framebuffer
            Framebuffer::doBindFramebuffer(0, (int)Gui::mfbWidth(), (int)Gui::mfbHeight());
        } else {
            Framebuffer::doBindFramebuffer(globalFboStack.top());
        }
    }




    void checkFramebufferStatus() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log::warn("ERR: Framebuffer is not complete");
        }
    }

    // disableColorBuffers();
    void disableDrawBuffers() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // tell GL render to which Color Attachments/Textures
    void setupMRT(std::initializer_list<int> l) {
        int n = l.size();
        unsigned int arr[n];
        Collections::range(arr, (uint)n, (uint)GL_COLOR_ATTACHMENT0);
        glDrawBuffers(n, arr);
    }

    // initMRT() ?

    // resize() ?

};


#endif //ETHERTIA_FRAMEBUFFER_H
