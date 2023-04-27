//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_SHADER_H
#define ETHERTIA_SHADER_H

#include <stdexcept>
#include <string>
#include <map>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <ethertia/util/Log.h>
#include <ethertia/util/Strings.h>
#include <ethertia/util/BenchmarkTimer.h>

#define DECL_SHADER(varname, decl) inline static ShaderProgram* varname = decl;


class ShaderProgram
{
public:
    GLuint m_ProgramId = 0;

    bool m_Good = false;  // Loaded.

    ~ShaderProgram() {
        glDeleteProgram(m_ProgramId);
    }


    // a high level func.
    void reload_sources_by_filenames();

    void load(const char* vsh_src, const char* fsh_src, const char* gsh_src = nullptr);



    int getProgramId() {
        return m_ProgramId;
    }
    void useProgram() {
        glUseProgram(m_ProgramId);

        updateBindUniform();
    }

    GLuint getUniformLocation(const char* name) {
        auto& unif = m_Uniforms[name];
        GLuint loc = unif.uniformId;
        if (!loc) {
            return unif.uniformId = glGetUniformLocation(m_ProgramId, name);
        }
        return loc;
    }

    void setVector2f(const char* name, glm::vec2 v) {
        glUniform2f(getUniformLocation(name), v.x, v.y);
    }
    void setVector3f(const char* name, glm::vec3 v) {
        glUniform3f(getUniformLocation(name), v.x, v.y, v.z);
    }
    void setVector4f(const char* name, glm::vec4 v) {
        glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
    }

    void setMatrix4f(const char* name, glm::mat4 m) {
        glUniformMatrix4fv(getUniformLocation(name), 1, false, &m[0][0]);
    }

    void setInt(const char* name, int i) {
        glUniform1i(getUniformLocation(name), i);
    }
    void setFloat(const char* name, float f) {
        glUniform1f(getUniformLocation(name), f);
    }

    void setViewProjection(bool view = true);
    void setMVP(const glm::mat4& matModel) {
        setMatrix4f("matModel", matModel);
        setViewProjection();
    }



    // GenArrayNames("chars[{}]", 128);
    static const char** _GenArrayNames(const std::string& namep, int n) {
        const char** arr = new const char*[n];
        for (int i = 0; i < n; ++i) {
            std::string s = Strings::fmt(namep, i);

            size_t size = s.length() + 1;  // +1 for \0
            char* data = new char[size];
            std::memcpy(data, s.c_str(), size);
            arr[i] = data;
        }
        return arr;
    }


    void updateBindUniform()
    {
        for (auto& it : m_Uniforms) {
            const char* name = it.first;
            auto& unif = it.second;
            float* valptr = (float*)unif.value_ptr;
            if (valptr) {
                switch (unif.type) {
                    case Uniform::INT:
                        setInt(name, *(int*)valptr);
                        break;
                    case Uniform::FLOAT:
                        setFloat(name, *valptr);
                        break;
                    case Uniform::VEC3:
                        setVector3f(name, {valptr[0], valptr[1], valptr[2]});
                        break;
                    case Uniform::VEC4:
                        setVector4f(name, {valptr[0], valptr[1], valptr[2], valptr[3]});
                        break;
                    default:
                        throw std::runtime_error("Unsupported uniform bind type");
                }
            }
        }
    }


    struct Uniform
    {
        enum Type {
            INT,
            FLOAT,
            VEC3,
            VEC4,
            MAT3,
            MAT4
        };

        GLuint uniformId = 0;
        ShaderProgram::Uniform::Type type;

        void* value_ptr = nullptr;

        void bind(Uniform::Type t, void* p) {
            value_ptr = p;
            type = t;
        }
        void bind(float* f32) {
            bind(FLOAT, f32);
        }
        void bind(glm::vec3* v3) {
            bind(VEC3, v3);
        }
        void bind(glm::vec4* v4) {
            bind(VEC4, v4);
        }

        // Range? Bind?
    };
    std::map<const char*, ShaderProgram::Uniform> m_Uniforms;


    // tmeporary. shader source file location. use as shader id.
    std::string m_SourceLocation;
    bool m_HaveGeomShader = false;



    inline static std::map<std::string, ShaderProgram*> REGISTRY;
    static ShaderProgram* decl(const std::string& pat, bool gsh = false)
    {
        ShaderProgram* p = new ShaderProgram();
        p->m_SourceLocation = pat;
        p->m_HaveGeomShader = gsh;
        REGISTRY[pat] = p;
        return p;
    }
    static void loadAll()
    {
        BENCHMARK_TIMER;
        Log::info("Loading all shader programs... ({})\1", ShaderProgram::REGISTRY.size());
        for (auto& it : ShaderProgram::REGISTRY) {
            it.second->reload_sources_by_filenames();
        }
    }

};


#endif //ETHERTIA_SHADER_H
