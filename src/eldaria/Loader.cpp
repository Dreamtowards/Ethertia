//
// Created by Dreamtowards on 2023/3/19.
//

#include "Loader.h"

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>


#include <ethertia/util/Strings.h>
#include <ethertia/util/Log.h>


Loader::DataBlock Loader::loadFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error(Strings::fmt("Failed open file. ", path));
    }
    size_t filesize = file.tellg();
    char* data = new char[filesize];

    file.seekg(0);
    file.read(data, filesize);
    file.close();

    return Loader::DataBlock(data, filesize, path);
}


BitmapImage Loader::loadPNG(const std::string& path)
{
    int w, h, channels;
    stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);

    return BitmapImage(pixels, w, h, path);
}


VertexData Loader::loadOBJ(const std::string& filename)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str())) {
        throw std::runtime_error(err);
    }
    //if (!err.empty()) { Log::warn("Error occur when loading .OBJ '{}': ", filename, err); }

    VertexData vertexdata{filename};

    for (tinyobj::shape_t& shape : shapes)
    {
        vertexdata.m_Vertices.reserve(vertexdata.m_Vertices.size() + shape.mesh.indices.size());

        for (tinyobj::index_t& index : shape.mesh.indices)
        {
            glm::vec3 pos = *reinterpret_cast<glm::vec3*>(&attrib.vertices[3*index.vertex_index]);
            glm::vec2 tex = *reinterpret_cast<glm::vec2*>(&attrib.texcoords[2*index.texcoord_index]);
            glm::vec3 norm = *reinterpret_cast<glm::vec3*>(&attrib.normals[3*index.normal_index]);

            // vulkan y 0=top
            tex.y = 1.0f - tex.y;

            vertexdata.m_Vertices.push_back({pos, {1,1,1}, tex});
        }
    }

//    for (const auto& shape : shapes) {
//        for (const auto& index : shape.mesh.indices) {
//            VertexData::Vertex vertex{};
//            vertex.pos = {
//                    attrib.vertices[3 * index.vertex_index + 0],
//                    attrib.vertices[3 * index.vertex_index + 1],
//                    attrib.vertices[3 * index.vertex_index + 2]
//            };
//            vertex.tex = {
//                    attrib.texcoords[2 * index.texcoord_index + 0],
//                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
//            };
//            vertex.norm = {1,1,1};
//
//            vertexdata.m_Vertices.push_back(vertex);
//        }
//    }
    return vertexdata;
}










BitmapImage::BitmapImage(void* pixels, int w, int h, const std::string& _filename)
        : m_Pixels(pixels), m_Width(w), m_Height(h), m_Filename(_filename)
{
    Log::info("New BitmapImage: {}", m_Filename);
}

BitmapImage::~BitmapImage()
{
    std::free(m_Pixels);

    Log::info("Delete BitmapImage: {}", m_Filename);
}




VertexData::VertexData(const std::string& _filename) : m_Filename(_filename)
{
    Log::info("New VertexData: {} with {} vertices", m_Filename, m_Vertices.size());
}
VertexData::~VertexData()
{
    Log::info("Delete VertexData: {} with {} vertices", m_Filename, m_Vertices.size());
}




Loader::DataBlock::DataBlock(void* data, size_t size, const std::string& filename)
    : m_Data(data), m_Size(size), m_Filename(filename)
{
    Log::info("New DataBlock: {} of {} bytes", m_Filename, size);
}

Loader::DataBlock::~DataBlock()
{
    std::free(m_Data);

    Log::info("Delete DataBlock: {}", m_Filename);
}






#include <GLFW/glfw3.h>

#include "Window.h"

#define UnpackGLFWwindow Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))

static void glfw_framebuffer_resized(GLFWwindow* glfwWindow, int w, int h)
{
    UnpackGLFWwindow;
    win->m_FramebufferResized = true;
}

Window::Window(int w, int h, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // disable OpenGL

    m_WindowHandle = glfwCreateWindow(w, h, title, nullptr, nullptr);

    glfwSetWindowUserPointer(m_WindowHandle, this);
    glfwSetFramebufferSizeCallback(m_WindowHandle, glfw_framebuffer_resized);
}

Window::~Window()
{
    glfwDestroyWindow(m_WindowHandle);
}

bool Window::isCloseRequested() {
    return glfwWindowShouldClose(m_WindowHandle);
}

void Window::ProcessEvents()
{
    m_FramebufferResized = false;

    glfwPollEvents();
}
