//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <eldaria/BitmapImage.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

class VertexData
{
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 norm;
        glm::vec2 tex;
    };

    VertexData(const std::string& _filename);
    ~VertexData();

    const void* data() const { return m_Vertices.data(); }
    size_t size() const { return sizeof(m_Vertices[0]) * m_Vertices.size(); }

    size_t vertexCount() const { return m_Vertices.size(); }

    std::vector<Vertex> m_Vertices;
    std::string m_Filename;  // optional. debug tag.
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



};

#endif //ETHERTIA_LOADER_H
