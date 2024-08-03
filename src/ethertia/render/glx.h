//
// Created by Dreamtowards on 2024/8/1.
//

#ifndef ETHERTIA_GLX_H
#define ETHERTIA_GLX_H

#include <stdint.h>
#include <glad-gl4.6/include/glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <map>
#include <functional>

#include "ethertia/util/Log.h"
#include "ethertia/util/Collections.h"

class Texture {
private:
    Texture() = default;
public:
//    Texture(int width, int height, GLuint textureId) : m_Width(width), m_Height(height), m_TextureId(textureId) {}

    int width()  const { return m_Width; }
    int height() const { return m_Height; }

    int id() const { return m_TextureId; }
//    int textureId() const { return m_TextureId; }

    void* idptr() const { return (void*)(intptr_t)m_TextureId; }
//    void* textureIdPtr() const { return (void*)(intptr_t)m_TextureId; }

    int target() const { return m_Target; }

    void Bind(int slot = 0) {
        glBindTextureUnit(slot, m_TextureId);
//        glActiveTexture(GL_TEXTURE0+slot);
//        glBindTexture(target, texId);
    }

    static Texture* Create(int w, int h, int target = GL_TEXTURE_2D) {
        auto* tex = new Texture();
        tex->m_Width = w;
        tex->m_Height = h;
        tex->m_Target = target;
        glCreateTextures(target, 1, &tex->m_TextureId);
        return tex;
    }

private:
    int m_Width{};
    int m_Height{};
    int m_Target{};  // e.g. GL_TEXTURE_2D

    GLuint m_TextureId{};
};


class VertexBufferArrays {
public:
    VertexBufferArrays(GLuint vaoId, GLuint vboId, GLuint iboId, size_t vertexCount) :
        m_vaoId(vaoId), m_vboId(vboId), m_iboId(iboId), m_VertexCount(vertexCount) {}

    bool indexed() const { return m_iboId != 0; }

    GLuint m_vaoId = 0;
    GLuint m_vboId = 0;
    GLuint m_iboId = 0;
    size_t m_VertexCount = 0;
};


// Instanced Shader
class ShaderProgram {
public:
    GLuint m_ProgramId;

    std::string m_SourcePath;  // for Reload

    ~ShaderProgram() {
        glDeleteProgram(m_ProgramId);
    }

    void Load(const char* vsh, const char* fsh);

    void Reload() {

    }

    void Bind() const {
        glUseProgram(m_ProgramId);
    }

    void SetInt(const char* name, int v) {
        glUniform1i(GetUniformId(name), v);
    }
    void SetFloat(const char* name, float v) {
        glUniform1f(GetUniformId(name), v);
    }

    void SetVec2(const char* name, glm::vec2 v) {
        glUniform2f(GetUniformId(name), v.x, v.y);
    }
    void SetVec3(const char* name, glm::vec3 v) {
        glUniform3f(GetUniformId(name), v.x, v.y, v.z);
    }
    void SetVec4(const char* name, glm::vec4 v) {
        glUniform4f(GetUniformId(name), v.x, v.y, v.z, v.w);
    }
    void SetMat4(const char* name, const glm::mat4& m) {
        glUniformMatrix4fv(GetUniformId(name), 1, false, &m[0][0]);
    }

    struct Uniform {
        GLint UniformId;
    };
    std::map<const char*, Uniform> m_Uniforms;

    GLint GetUniformId(const char* name) {
        auto& u = m_Uniforms[name];
        GLint loc = u.UniformId;
        if (!loc) {
            return u.UniformId = glGetUniformLocation(m_ProgramId, name);
        }
        return loc;
    }

    inline static std::vector<ShaderProgram*> REGISTRY;

    static ShaderProgram* Create(const std::string& srcpath, const std::function<void(ShaderProgram&)>& initfunc) {
        auto shader = new ShaderProgram();
        shader->m_SourcePath = srcpath;
        shader->Reload();
        shader->Bind();
        initfunc(*shader);
        REGISTRY.push_back(shader);
        return shader;
    }
};



class Framebuffer
{
private:
    Framebuffer() = default;  // should explicitly create FBO. use Framebuffer::glfGenFramebuffer();
public:
    int m_fboId  = 0;
    int m_Width  = 0;
    int m_Height = 0;

    // Texture Attachments.
    Texture* m_TexColor[8]     = {};
    Texture* m_TexDepth        = nullptr;
    Texture* m_TexDepthStencil = nullptr;  // RBO?

    static Framebuffer* Create(int w, int h, const std::function<void(Framebuffer&)>& initfunc);
    ~Framebuffer() {
        glDeleteFramebuffers(1, (GLuint*)&m_fboId);
    }

    int fboId() const { return m_fboId; }
    int width() const { return m_Width; }
    int height() const { return m_Height; }


    void AttachColorTexture(int i, int intlfmt = GL_RGB, int fmt = GL_RGB, int type = GL_UNSIGNED_BYTE) {
        m_TexColor[i] = InternalAttachTexture2D(GL_COLOR_ATTACHMENT0+i, intlfmt, fmt, type);
    }
    void AttachDepthTexture() {
        m_TexDepth = InternalAttachTexture2D(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    }
    void AttachDepthStencilTexture() {
        m_TexDepthStencil = InternalAttachTexture2D(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    }
    Texture* InternalAttachTexture2D(int attachment, int intlfmt, int fmt, int type);


    void AttachDepthStencilRenderbuffer() {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    static void BeginFramebuffer(Framebuffer* fbo);
    static void EndFramebuffer();


    // use for Scoped push/pop. for Profiler, Framebuffer
    class DtorCaller
    {
    public:
        std::function<void()> fn;
        ~DtorCaller()
        {
            fn();
        }
    };

    // auto pop
    [[nodiscard]]
    DtorCaller BindScoped() {
        Framebuffer::BeginFramebuffer(this);

        return DtorCaller{[](){
            Framebuffer::EndFramebuffer();
        }};
    }




    void CheckStatus() {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Log::warn("ERR: Framebuffer is not complete");
        }
    }

    // disableColorBuffers();  use for e.g. ShadowMapping render that only need depth texture.
    void DisableDrawBuffers() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // tell GL render to which Color Attachments/Textures
    void SetupMRT(std::initializer_list<int> l) {
        size_t n = l.size();
        std::vector<GLuint> arr(n);
        Collections::range(arr.data(), n, (GLuint)GL_COLOR_ATTACHMENT0);
        glDrawBuffers(n, arr.data());
    }

    // resize() ?

};


#endif //ETHERTIA_GLX_H
