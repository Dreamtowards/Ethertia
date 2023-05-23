//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <eldaria/util/BitmapImage.h>
#include <eldaria/vulkan/VertexBuffer.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>


class VertexData
{
public:
    class Vertex
    {
    public:
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 norm;


        bool operator==(const Vertex& o) const {
            return pos == o.pos && tex == o.tex && norm == o.norm;
        }
    };

    VertexData(const std::string& _filename);
    ~VertexData();

    const void* vtx_data() const { return m_Vertices.data(); }
    size_t vtx_size() const { return sizeof(m_Vertices[0]) * m_Vertices.size(); }

    const uint32_t* idx_data() const { return m_Indices.data(); }
    size_t idx_size() const { return sizeof(m_Indices[0]) * m_Indices.size(); }


    size_t vertexCount() const { return m_Indices.empty() ? m_Vertices.size() : m_Indices.size(); }

    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    std::string m_Filename;  // dbg.
};

class Loader
{
public:
    struct DataBlock
    {
        const void* data() const { return m_Data; }
        size_t size() const { return m_Size; }

        DataBlock(void* data, size_t size, const std::string& filename);
        ~DataBlock();

        void*       m_Data;
        size_t      m_Size;
        std::string m_Filename;  // optional. debug tag.
    };

    // load entire file. filename could be absolute-path or relative-path.
    static DataBlock loadFile(const std::string& filename);
    // fixme: use std::vector<char> or Special DataBlock class?


    // backend: stb_image. supports PNG, JPG, GIF, TGA, BMP, HDR, PSD, PIC, PPM/PGM.
    static BitmapImage loadPNG(const std::string& filename);
    // fixme: return ptr? std::shared_ptr? or just object?


    // backend: tiny_obj_loader.
    static VertexData loadOBJ(const std::string& filename);



    static vkx::VertexBuffer* loadVertexBuffer(const VertexData& vdata);

    static vkx::Image* loadImage(const BitmapImage& bitmapImage);


    static std::string fileResolve(const std::string& uri) {
        return "./assets/"+uri;
    }


};

#endif //ETHERTIA_LOADER_H
