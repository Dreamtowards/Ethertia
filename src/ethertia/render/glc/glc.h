//
// Created by Dreamtowards on 2023/5/12.
//

#ifndef ETHERTIA_GLC_H
#define ETHERTIA_GLC_H

#include <cstdint>
#include <functional>

typedef uint32_t GLenum;
typedef uint32_t GLuint;

namespace glc
{

    class Texture
    {
    public:
        GLuint textureId;
        GLuint target;

        int width = 0;
        int height = 0;
    };

    // glGenTextures() glBindTexture()
    Texture* CreateTexture(GLenum target);

    void TextureParameter(Texture* tex, GLenum pname, GLenum param);

    // glActiveTexture(); glBindTexture();
    void BindTextureUnit(int unit, Texture* tex);





    //////////////// glDebugMessage, GL 4.3+ ////////////////

    const char* GetString_DebugMessageEnum(GLenum src_type_serv);

    struct DebugMessageCallbackArgs
    {
        GLenum source;
        GLenum type;
        GLuint id;
        GLenum severity;
        size_t length;
        const char* message;
        const void* user_param;

        const char* source_str;
        const char* type_str;
        const char* severity_str;
    };

    // return true: successfully setup. (valid debug context)
    bool DebugMessageCallback(const std::function<void(const glc::DebugMessageCallbackArgs&)>& callback, bool sync = true);


    class VertexArray
    {
    public:
        GLuint vaoId;
        GLuint vboId;       // interleaved vertex data
        GLuint iboId = 0;   // optional, 0: non-indexed.
        uint32_t vertexCount = 0;

        VertexArray(GLuint vaoId, GLuint vboId, GLuint iboId, uint32_t vertexCount) :
                    vaoId(vaoId), vboId(vboId), iboId(iboId), vertexCount(vertexCount) {}

        [[nodiscard]]
        bool indexed() const { return iboId != 0; }
    };


    // no-idx:  load(vc, {3,2,3}, vtx);
    // idx:     load(vc, {3,2,3}, vtx_data, vtx_size, idx_data);
    VertexArray* loadVertexData(uint32_t vertexCount, std::initializer_list<int> attrib_sizes, float* vtx_data,
                                uint32_t vtx_size = -1, uint32_t* idx_data = nullptr);





    class ShaderProgram
    {
        GLuint programId;


    };


}

#endif //ETHERTIA_GLC_H
