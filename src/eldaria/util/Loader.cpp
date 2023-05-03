//
// Created by Dreamtowards on 2023/3/19.
//

//#include "Loader.h"

//#include <unordered_map>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>
//
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader/tiny_obj_loader.h>
//
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/hash.hpp>
//
//
//#include <ethertia/util/Strings.h>
//#include <ethertia/util/Log.h>
//
//
//
//namespace std {
//    template<> struct hash<VertexData::Vertex> {
//        size_t operator()(VertexData::Vertex const& vertex) const {
//            return ((hash<glm::vec3>()(vertex.pos) ^
//                    (hash<glm::vec2>()(vertex.tex)  << 1)) >> 1) ^
//                    (hash<glm::vec3>()(vertex.norm) << 1);
//        }
//    };
//}
//
//
//Loader::DataBlock Loader::loadFile(const std::string& path)
//{
//    std::ifstream file(path, std::ios::binary | std::ios::ate);
//    if (!file.is_open()) {
//        throw std::runtime_error(Strings::fmt("Failed open file. ", path));
//    }
//    size_t filesize = file.tellg();
//    char* data = new char[filesize];
//
//    file.seekg(0);
//    file.read(data, filesize);
//    file.close();
//
//    return Loader::DataBlock(data, filesize, path);
//}
//
//
//BitmapImage Loader::loadPNG(const std::string& path)
//{
//    int w, h, channels;
//    stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
//    assert(pixels);
//
//    return BitmapImage(pixels, w, h, path);
//}
//
//// Arrays vs Indexed compare for viking_room.obj (Single Vertex is vec3+vec2+vec3 8*f32 = 32 bytes, Index is uint32 = 4 bytes)
//// (arrays: 11484 vert *32 = 367,488 bytes) dbg-run 18ms
//// (indexed: 4725 unique vert *32 = 151,200 bytes (=x0.41) + 11484 indices * 4 = 45,936 bytes  =  197,136 bytes (=x0.54)) dbg-run 21ms
//VertexData Loader::loadOBJ(const std::string& filename)
//{
//    tinyobj::attrib_t attrib;
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    std::string err;
//
//    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str())) {
//        throw std::runtime_error(err);
//    }
//    //if (!err.empty()) { Log::warn("Error occur when loading .OBJ '{}': ", filename, err); }
//
//    VertexData vtx{filename};
//
//    std::unordered_map<VertexData::Vertex, uint32_t> unique_verts;
//
//    for (tinyobj::shape_t& shape : shapes)
//    {
//        vtx.m_Vertices.reserve(vtx.m_Vertices.size() + shape.mesh.indices.size());
//
//        for (tinyobj::index_t& index : shape.mesh.indices)
//        {
//            glm::vec3 pos = *reinterpret_cast<glm::vec3*>(&attrib.vertices[3*index.vertex_index]);
//            glm::vec2 tex = *reinterpret_cast<glm::vec2*>(&attrib.texcoords[2*index.texcoord_index]);
//            glm::vec3 norm = *reinterpret_cast<glm::vec3*>(&attrib.normals[3*index.normal_index]);
//            VertexData::Vertex vert = {pos, tex, norm};
//
//            // vulkan y 0=top
//            tex.y = 1.0f - tex.y;
//
//            if (unique_verts.find(vert) == unique_verts.end())
//            {
//                unique_verts[vert] = vtx.m_Vertices.size();
//                vtx.m_Vertices.push_back(vert);
//            }
//            vtx.m_Indices.push_back(unique_verts[vert]);
//        }
//    }
//    return vtx;
//}
//
//
//
//
//
//
//
//#include <eldaria/vulkan/vkh.h>
//
//
//vkx::VertexBuffer* Loader::loadVertexBuffer(const VertexData& vdata)
//{
//    VkBuffer vtxBuffer, idxBuffer;
//    VkDeviceMemory vtxMemory, idxMemory;
//
//    vkx::CreateStagedBuffer(vdata.vtx_data(), vdata.vtx_size(), &vtxBuffer, &vtxMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
//    vkx::CreateStagedBuffer(vdata.idx_data(), vdata.idx_size(), &idxBuffer, &idxMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
//
//    int vcount = vdata.vertexCount();
//    return new vkx::VertexBuffer(vtxBuffer, vtxMemory, idxBuffer, idxMemory, vcount);
//}
//
//vkx::Image* Loader::loadImage(const BitmapImage& bitmapImage)
//{
//    VkImage image;
//    VkDeviceMemory imageMemory;
//    VkImageView imageView;
//    vkx::CreateStagedImage(bitmapImage.m_Width, bitmapImage.m_Height, bitmapImage.m_Pixels,
//                           &image, &imageMemory, &imageView);
//
//    return new vkx::Image(image, imageMemory, imageView, bitmapImage.m_Width, bitmapImage.m_Height);
//}
//
//
//
//
//
//
//
//
//BitmapImage::BitmapImage(void* pixels, int w, int h, const std::string& _filename)
//        : m_Pixels(pixels), m_Width(w), m_Height(h), m_Filename(_filename)
//{
//    Log::info("New BitmapImage: {}", m_Filename);
//}
//
//BitmapImage::~BitmapImage()
//{
//    std::free(m_Pixels);
//
//    Log::info("Delete BitmapImage: {}", m_Filename);
//}
//
//
//
//
//
//
//
//
//
//VertexData::VertexData(const std::string& _filename) : m_Filename(_filename)
//{
//    Log::info("New VertexData: {}", m_Filename);
//}
//VertexData::~VertexData()
//{
//    Log::info("Delete VertexData: {} with {} vertices, {} indices", m_Filename, m_Vertices.size(), m_Indices.size());
//}
//
//
//
//
//Loader::DataBlock::DataBlock(void* data, size_t size, const std::string& filename)
//    : m_Data(data), m_Size(size), m_Filename(filename)
//{
//    Log::info("New DataBlock: {} of {} bytes", m_Filename, size);
//}
//
//Loader::DataBlock::~DataBlock()
//{
//    std::free(m_Data);
//    Log::info("Delete DataBlock: {}", m_Filename);
//}
//

//#include <fstream>









#include <GLFW/glfw3.h>

#include <eldaria/Window.h>
#include <ethertia/util/Loader.h>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void Window::Init()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("failed to init glfw.");

    if (!glfwVulkanSupported())
        throw std::runtime_error("GLFW: Vulkan not supported.");
}

void Window::Destroy()
{
    glfwTerminate();
}

double Window::Time()
{
    return glfwGetTime();
}

#define UnpackGLFWwindow Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))

static void glfw_framebuffer_resized(GLFWwindow* glfwWindow, int w, int h)
{
    UnpackGLFWwindow;
    win->m_FramebufferResized = true;
}

Window::Window(int w, int h, const char* title)
{
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
bool Window::isFramebufferResized() {
    return m_FramebufferResized;
}

// PollEvents / HandleEvents / ProcessEvents
void Window::ProcessEvents()
{
    // reset delta states
    m_FramebufferResized = false;

    glfwPollEvents();
}


bool Window::isKeyDown(int key) {
    return glfwGetKey(m_WindowHandle, key) == GLFW_PRESS;
}
bool Window::isMouseDown(int mb) {
    return glfwGetMouseButton(m_WindowHandle, mb) == GLFW_PRESS;
}


bool Window::isCtrlKeyDown() { return isKeyDown(GLFW_KEY_LEFT_CONTROL) || isKeyDown(GLFW_KEY_RIGHT_CONTROL); }
bool Window::isShiftKeyDown() { return isKeyDown(GLFW_KEY_LEFT_SHIFT) || isKeyDown(GLFW_KEY_RIGHT_SHIFT); }
bool Window::isAltKeyDown() { return isKeyDown(GLFW_KEY_LEFT_ALT) || isKeyDown(GLFW_KEY_RIGHT_ALT); }

bool Window::isMouseLeftDown() { return isMouseDown(GLFW_MOUSE_BUTTON_LEFT); }
bool Window::isMouseMiddleDown() { return isMouseDown(GLFW_MOUSE_BUTTON_MIDDLE); }
bool Window::isMouseRightDown() { return isMouseDown(GLFW_MOUSE_BUTTON_RIGHT); }
