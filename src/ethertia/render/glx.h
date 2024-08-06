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
#include "ethertia/util/BitmapImage.h"
#include "ethertia/util/Colors.h"
#include "VertexData.h"

namespace glx
{
    void Clear(glm::vec4 color = Colors::BLACK);

    GLuint CheckError(std::string_view phase);

    void InitDebugOutput();
}

class Texture {
private:
    Texture() = default;
public:
    static Texture* Create(int w, int h, int target = GL_TEXTURE_2D, int allocInternalFormat = 0) {
        auto* tex = new Texture();
        tex->m_Width = w;
        tex->m_Height = h;
        tex->m_Target = target;
        glCreateTextures(target, 1, &tex->m_TextureId);
        if (allocInternalFormat != 0) {
            tex->Alloc(allocInternalFormat);
            tex->SetWrap();
            tex->SetFilter();
        }
        return tex;
    }
    ~Texture() {
        glDeleteTextures(1, &m_TextureId);
    }

    Texture* Alloc(int internalformat = GL_RGBA8) {
        if (m_Target == GL_TEXTURE_2D) {
            glTextureStorage2D(m_TextureId, 1, internalformat, m_Width, m_Height);
        } else {
            throw std::logic_error("Not supported");
        }
        return this;
    }
    void GenerateMipmap() {
        glGenerateTextureMipmap(m_TextureId);
    }

    [[nodiscard]] int width()  const { return m_Width; }
    [[nodiscard]] int height() const { return m_Height; }

    [[nodiscard]] int id() const { return m_TextureId; }
//    int textureId() const { return m_TextureId; }

    [[nodiscard]] void* idptr() const { return (void*)(intptr_t)m_TextureId; }
//    void* textureIdPtr() const { return (void*)(intptr_t)m_TextureId; }

    [[nodiscard]] int target() const { return m_Target; }

    void Bind(int slot = 0) const {
        glBindTextureUnit(slot, m_TextureId);
//        glActiveTexture(GL_TEXTURE0+slot);
//        glBindTexture(target, texId);
    }

    void SetWrap(int wrap = GL_REPEAT) {
        glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, wrap);
    }
    void SetFilter(int filter = GL_NEAREST) {  //GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST
        glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, filter);
        glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, filter);
    }

    BitmapImage* GetTexImage() {
        void* pixels = new char[m_Width * m_Height * 4];
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        return new BitmapImage(m_Width, m_Height, (unsigned int*)pixels);
    }

private:
    int m_Width{};
    int m_Height{};
    int m_Target{};  // e.g. GL_TEXTURE_2D

    GLuint m_TextureId{};
};


class VertexArrays {
    VertexArrays(GLuint vaoId, GLuint vboId, GLuint iboId, size_t vertexCount) :
        m_vaoId(vaoId), m_vboId(vboId), m_iboId(iboId), m_VertexCount(vertexCount) {}
public:

    [[nodiscard]] bool indexed() const { return m_iboId != 0; }

    GLuint m_vaoId = 0;
    GLuint m_vboId = 0;
    GLuint m_iboId = 0;
    size_t m_VertexCount = 0;

    void BindAndDraw() {
        glBindVertexArray(m_vaoId);
        if (indexed()) {
            glDrawElements(GL_TRIANGLES, m_VertexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
        }
    }

    static VertexArrays* Load(VertexData* vbuf) {
        if (vbuf->IsIndexed()) {
            return Load(vbuf->VertexCount(), {3,2,3}, (const float*)vbuf->vtx_data(),
                        vbuf->vtx_size(),(const uint32_t*)vbuf->idx_data());
        } else {
            return Load(vbuf->VertexCount(), {3,2,3}, (const float*)vbuf->vtx_data());
        }
    }

    // attrib_sizes: Interleaved Num Scalars for Each Vertex. e.g. {3,2,3} for {pos, uv, norm}
    static VertexArrays* Load(
            uint32_t vertexCount, std::initializer_list<int> attrib_sizes,
            const float* vtx_data, uint32_t vtx_size = -1, const uint32_t* idx_data = nullptr)
    {
        assert(attrib_sizes.size() > 0);
        int stride = 0;
        for (int s : attrib_sizes) { stride += s; }

        GLuint vaoId;
        glCreateVertexArrays(1, &vaoId);

        GLuint iboId = 0;
        if (idx_data) {
            assert(vtx_size > 0);

            uint32_t idx_size = sizeof(uint32_t) * vertexCount;
            glCreateBuffers(1, &iboId);
            glNamedBufferStorage(iboId, idx_size, idx_data, GL_DYNAMIC_STORAGE_BIT);
            glVertexArrayElementBuffer(vaoId, iboId);
        } else {
            assert(vtx_size == -1);
            vtx_size = stride * sizeof(float) * vertexCount;
        }

        GLuint vboId;
        glCreateBuffers(1, &vboId);
        glNamedBufferStorage(vboId, vtx_size, vtx_data, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(vaoId, 0, vboId, 0, stride * sizeof(float));

        int offset = 0;
        int attrib  = 0;
        for (int attrib_size : attrib_sizes)
        {
            glVertexArrayAttribFormat(vaoId, attrib, attrib_size, GL_FLOAT, GL_FALSE, offset*sizeof(float));
            glVertexArrayAttribBinding(vaoId, attrib, 0);
            glEnableVertexArrayAttrib(vaoId, attrib);

            offset += attrib_size;
            ++attrib;
        }

//        GLuint vaoId;
//        glGenVertexArrays(1, &vaoId);
//        glBindVertexArray(vaoId);
//
//        int _scalars = 0;
//        for (int s : attrib_sizes) { _scalars += s; }
//        int stride = _scalars * sizeof(float);
//
//        GLuint iboId = 0;
//        if (idx_data)
//        {
//            glGenBuffers(1, &iboId);
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
//            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * vertexCount, idx_data, GL_STATIC_DRAW);
//        } else {
//            vtx_size = stride*vertexCount;
//        }
//
//        GLuint vboId;
//        glGenBuffers(1, &vboId);
//        glBindBuffer(GL_ARRAY_BUFFER, vboId);
//        glBufferData(GL_ARRAY_BUFFER, vtx_size, vtx_data, GL_STATIC_DRAW);
//
//        int i = 0;
//        _scalars = 0;
//        for (int s : attrib_sizes) {
//            glVertexAttribPointer(i, s, GL_FLOAT, GL_FALSE, stride, (void*)(_scalars*sizeof(float)));
//            glEnableVertexAttribArray(i);
//            ++i;
//            _scalars += s;
//        }

        return new VertexArrays(vaoId, vboId, iboId, vertexCount);
    }
};


// Instanced Shader
class ShaderProgram {
public:
    GLuint m_ProgramId;

    std::string m_SourcePath;  // for Reload

    ~ShaderProgram() {
        glDeleteProgram(m_ProgramId);
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
        int UniformId;
    };
    std::map<const char*, Uniform> m_Uniforms;

    int GetUniformId(const char* name) {
        auto& u = m_Uniforms[name];
        int loc = u.UniformId;
        if (!loc) {
            return u.UniformId = glGetUniformLocation(m_ProgramId, name);
        }
        return loc;
    }

    inline static std::vector<ShaderProgram*> REGISTRY;

    static ShaderProgram* Create(const std::string& srcpath, const std::function<void(ShaderProgram&)>& initfunc) {
        auto* shader = new ShaderProgram();
        shader->m_ProgramId = glCreateProgram();

        shader->m_SourcePath = srcpath;
        shader->Reload();

        shader->Bind();
        initfunc(*shader);
        REGISTRY.push_back(shader);
        return shader;
    }

    static const char* StrShaderType(GLuint shaderType) {
        switch (shaderType) {
            case GL_VERTEX_SHADER: return "vertex";
            case GL_FRAGMENT_SHADER: return "fragment";
            case GL_GEOMETRY_SHADER: return "geometry";
            default: return "Unknown";
        }
    }
    static GLuint LoadShader(GLuint shaderType, const char* src, const char* errIden) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int succ;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &succ);
        if (!succ) {
            char infolog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infolog);
            Log::info("Failed to compile the {} shader [{}]:\n", StrShaderType(shaderType), errIden, infolog);
            return 0;
        }
        return shader;
    }

    bool Load(const char* vs_src, const char* fs_src) {
        bool succ = true;

        GLuint vsh = LoadShader(GL_VERTEX_SHADER, vs_src, m_SourcePath.c_str());   if (!vsh) succ = false;
        GLuint fsh = LoadShader(GL_FRAGMENT_SHADER, fs_src, m_SourcePath.c_str()); if (!fsh) succ = false;

        glAttachShader(m_ProgramId, vsh);
        glAttachShader(m_ProgramId, fsh);

        glLinkProgram(m_ProgramId);

        int stat; glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &stat);
        if (!stat) {
            char infolog[512];
            glGetProgramInfoLog(m_ProgramId, 512, nullptr, infolog);
            Log::warn("Failed to link the shader program [{}]:\n", m_SourcePath, infolog);
            succ = false;
        }

        glDeleteShader(vsh);
        glDeleteShader(fsh);
        return succ;
    }

    void Reload();
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

    static Framebuffer* Create(int w, int h, const std::function<void(Framebuffer&)>& initfunc)
    {
        auto* fbo = new Framebuffer();
        fbo->m_Width = w;
        fbo->m_Height = h;
        glCreateFramebuffers(1, (GLuint*)&fbo->m_fboId);

        Framebuffer::BeginFramebuffer(fbo);
        initfunc(*fbo);
        fbo->CheckStatus();
        Framebuffer::EndFramebuffer();
        return fbo;
    }
    ~Framebuffer() {
        glDeleteFramebuffers(1, (GLuint*)&m_fboId);
    }

    [[nodiscard]] int fboId() const { return m_fboId; }
    [[nodiscard]] int width() const { return m_Width; }
    [[nodiscard]] int height() const { return m_Height; }


    void AttachColorTexture(int i, int internalformat = GL_RGB) {
        m_TexColor[i] = InternalAttachTexture2D(GL_COLOR_ATTACHMENT0+i, internalformat);
    }
    void AttachDepthTexture() {
        m_TexDepth = InternalAttachTexture2D(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT);
    }
    void AttachDepthStencilTexture() {
        m_TexDepthStencil = InternalAttachTexture2D(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8);
    }
    Texture* InternalAttachTexture2D(int attachment, int internalformat) {
        Texture* tex = Texture::Create(m_Width, m_Height, GL_TEXTURE_2D, internalformat);
        glNamedFramebufferTexture(m_fboId, attachment, tex->id(), 0);
        return tex;
    }

    void AttachDepthStencilRenderbuffer() const {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    static void BindMainFramebuffer();

    static void BeginFramebuffer(Framebuffer* fbo);
    static void EndFramebuffer();

    void Bind(int target = GL_FRAMEBUFFER) {
        glBindFramebuffer(target, m_fboId);
        glViewport(0, 0, m_Width, m_Height);
    }

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


    void ClearColor(int attachment, glm::vec4 color = Colors::BLACK) {
        glClearNamedFramebufferfv(m_fboId, GL_COLOR, attachment, &color[0]);
    }
    void ClearDepth(float depth = 1.0f) {
        glClearNamedFramebufferfv(m_fboId, GL_DEPTH, 0, &depth);
    }
    void ClearStencil(float stencil) {
        glClearNamedFramebufferfv(m_fboId, GL_STENCIL, 0, &stencil);
    }
    void ClearDepthStencil(float depth, float stencil) {
        glClearNamedFramebufferfi(m_fboId, GL_DEPTH_STENCIL, 0, depth, stencil);
    }


    void Blit(Framebuffer* dst, int mask, int filter,
              int srcX0=-1, int srcY0=-1, int srcX1=-1, int srcY1=-1,
              int dstX0=-1, int dstY0=-1, int dstX1=-1, int dstY1=-1) {
#define _N1OR(x, v) (x != -1 ? x : v)
        glBlitNamedFramebuffer(m_fboId, dst->m_fboId,
                               _N1OR(srcX0, 0), _N1OR(srcY0, 0), _N1OR(srcX1, 0), _N1OR(srcY1, 0),
                               _N1OR(dstX0, 0), _N1OR(dstX0, 0), _N1OR(dstX0, 0), _N1OR(dstX0, 0),
                               mask, filter);
    }


    void CheckStatus() {
        int status = glCheckNamedFramebufferStatus(m_fboId, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Log::info("ERR: Framebuffer is not complete. status: {}", status);
        }
    }

    // disableColorBuffers();  use for e.g. ShadowMapping render that only need depth texture.
    static void DisableDrawBuffers() {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // SetMRT. tell GL render to which Color Attachments/Textures
    static void SetDrawBuffers(std::initializer_list<int> l) {
        size_t n = l.size();
        std::vector<GLuint> arr(n);
        Collections::range(arr.data(), n, (GLuint)GL_COLOR_ATTACHMENT0);
        glDrawBuffers(n, arr.data());
    }

    // resize() ?

};


#endif //ETHERTIA_GLX_H
