//
// Created by Dreamtowards on 2023/2/1.
//

#include "Loader.h"

#include <format>
#include <fstream>
#include <filesystem>
#include <vkx/vkx.hpp>

#include <ethertia/util/Log.h>



#pragma region File, Asset


#define ET_LOADER_LOADINFO 1

Loader::DataBlock::DataBlock(void* data, size_t size, const std::string& filename)
        : m_Data(data), m_Size(size), m_Filename(filename)
{
#ifdef ET_LOADER_LOADINFO
    Log::info("New DataBlock: {} of {} bytes", m_Filename, size);
#endif
}

Loader::DataBlock::~DataBlock()
{
    std::free(m_Data);
#ifdef ET_LOADER_LOADINFO
    Log::info("Delete DataBlock: {}", m_Filename);
#endif
}

const void* Loader::DataBlock::data() const {
    return m_Data;
}
size_t Loader::DataBlock::size() const {
    return m_Size;
}
Loader::DataBlock::operator std::span<const char>() const {
    return std::span<const char>((const char*)data(), (int)size());
}






Loader::DataBlock Loader::LoadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: ", filename));
    }
    size_t filesize = (size_t)file.tellg();
    char* data = new char[filesize + 1];  // +1: add an extra '\0' at the end. for c_string format compatible.
    data[filesize] = '\0';  // set last extra byte to '\0'.

    file.seekg(0);
    file.read(data, filesize);
    file.close();

    return Loader::DataBlock(data, filesize, filename);
}

std::string Loader::FindAsset(const std::string& p)
{
    if (p.starts_with('.') || p.starts_with('/') || (p.length() >= 2 && p.find(':') == 1))
        return p;
    //for (const std::string& basepath : Settings::ASSETS)
    {
        std::string path = "assets/" + p;//basepath + p;
        if (Loader::FileExists(path))
            return path;
    }
    return "";
}



bool Loader::FileExists(const std::string& path)
{
    return std::filesystem::exists(path);
}

const std::string& Loader::Mkdirs(const std::string& path)
{
    // mkdirs for parents of the file.
    size_t _dir = path.rfind('/');
    if (_dir != std::string::npos) {
        std::filesystem::create_directories(path.substr(0, _dir));
    }
    return path;
}

size_t Loader::FileSize(const std::string& path)
{
    if (!std::filesystem::exists(path))
        return 0;

    if (std::filesystem::is_directory(path))
    {
        size_t sum = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (!entry.is_directory()) {
                sum += entry.file_size();
            }
        }
        return sum;
    }
    else
    {
        return std::filesystem::file_size(path);
    }
}


#pragma endregion


#pragma region PNG Load/Save


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>




BitmapImage Loader::LoadPNG_(const char* filename)
{
    // stbi_set_flip_vertically_on_load(true);
    int w, h, channels;
    void* pixels = stbi_load(filename, &w, &h, &channels, 4);
    assert(pixels);

    return BitmapImage(w, h, pixels);
}


BitmapImage Loader::LoadPNG(const void* data, size_t len)
{
    int w, h, channels;
    void* pixels = stbi_load_from_memory((unsigned char*)data, len, &w, &h, &channels, 4);

    return BitmapImage(w, h, pixels);
}

void Loader::SavePNG(const std::string& filename, const BitmapImage& img)
{
    Loader::Mkdirs(filename);
    if (!stbi_write_png(filename.c_str(), img.width(), img.height(), 4, img.pixels(), 0)) {
        throw std::runtime_error("Failed to write PNG. "+filename);
    }
}


#pragma endregion


#pragma region OBJ Load/Save


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>


// Arrays vs Indexed compare for viking_room.obj (Single Vertex is vec3+vec2+vec3 8*f32 = 32 bytes, Index is uint32 = 4 bytes)
// (arrays: 11484 vert *32 = 367,488 bytes) dbg-run 18ms
// (indexed: 4725 unique vert *32 = 151,200 bytes (=x0.41) + 11484 indices * 4 = 45,936 bytes  =  197,136 bytes (=x0.54)) dbg-run 21ms

VertexData* Loader::LoadOBJ_(const char* filename)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename)) {
        throw std::runtime_error(err);
    }
    // else if (!err.empty()) { Log::warn("Warn loading .OBJ '{}': {}", filename, err); }

    VertexData* vtx = new VertexData();
    vtx->m_Filename = filename;

    std::unordered_map<VertexData::Vertex, uint32_t> unique_verts;

    for (tinyobj::shape_t& shape : shapes)
    {
        vtx->Vertices.reserve(vtx->Vertices.size() + shape.mesh.indices.size());

        for (tinyobj::index_t& index : shape.mesh.indices)
        {
            glm::vec3 pos = *reinterpret_cast<glm::vec3*>(&attrib.vertices[3*index.vertex_index]);
            glm::vec2 tex = *reinterpret_cast<glm::vec2*>(&attrib.texcoords[2*index.texcoord_index]);
            glm::vec3 norm = *reinterpret_cast<glm::vec3*>(&attrib.normals[3*index.normal_index]);
            VertexData::Vertex vert = {pos, tex, norm};

            // for vulkan y 0=top, while wavefront obj y 0=bottom (opengl like)
            tex.y = 1.0f - tex.y;

            if (unique_verts.find(vert) == unique_verts.end())
            {
                unique_verts[vert] = vtx->Vertices.size();
                vtx->Vertices.push_back(vert);
            }
            vtx->Indices.push_back(unique_verts[vert]);
        }
    }
#ifdef ET_LOADER_LOADINFO
    Log::info("Load OBJ {}. VertexCount={}, VerticesTable={}. (vertices reduced to {}%, by additional {}% indices)",
              filename, vtx->VertexCount(), vtx->Vertices.size(),
              (float)vtx->Vertices.size()/vtx->VertexCount(),
              (float)vtx->Indices.size()/vtx->VertexCount()*((float)sizeof(uint32_t) / sizeof(VertexData::Vertex)));
#endif
    return vtx;
}




static void _SaveOBJ(std::stringstream& out, size_t verts, const float* pos, const float* uv, const float* norm)
{
    for (int vi = 0; vi < verts; ++vi) {
        int _b = vi * 3;
        out << "v " << pos[_b] << " " << pos[_b + 1] << " " << pos[_b + 2] << "\n";
    }

    if (uv) {
        for (int vi = 0; vi < verts; ++vi) {
            int _b = vi * 2;
            out << "vt " << uv[_b] << " " << uv[_b + 1] << "\n";
        }
    }

    if (norm) {
        for (int vi = 0; vi < verts; ++vi) {
            int _b = vi * 3;
            out << "vn " << norm[_b] << " " << norm[_b + 1] << " " << norm[_b + 2] << "\n";
        }
    }

    for (int i = 0; i < verts; i += 3) {
        const int _i = i + 1; // global index offset 1, obj spec.

        out << "f "
            << _i << "/" << _i << "/" << _i << " "
            << _i + 1 << "/" << _i + 1 << "/" << _i + 1 << " "
            << _i + 2 << "/" << _i + 2 << "/" << _i + 2 << "\n";
    }
}


void Loader::SaveOBJ(const std::string& filename, size_t verts, const float* pos, const float* uv, const float* norm)
{
    std::stringstream ss;
    _SaveOBJ(ss, verts, pos, uv, norm);

    Loader::Mkdirs(filename);
    std::ofstream fs(filename);
    fs << ss.str();
    fs.close();
}

#pragma endregion


#pragma region Vulkan: VertexBuffer, Image



vkx::VertexBuffer* Loader::LoadVertexData(const VertexData* vtx)
{
    vk::DeviceMemory vtxmem;
    vk::Buffer vtxbuf = vkx::CreateStagedBuffer(vtx->vtx_data(), vtx->vtx_size(), vtxmem, vk::BufferUsageFlagBits::eVertexBuffer);

    vk::DeviceMemory idxmem{};
    vk::Buffer idxbuf{};
    if (vtx->IsIndexed())
    {
        idxbuf = vkx::CreateStagedBuffer(vtx->idx_data(), vtx->idx_size(), idxmem, vk::BufferUsageFlagBits::eIndexBuffer);
    }
    return new vkx::VertexBuffer(vtxbuf, vtxmem, idxbuf, idxmem, vtx->VertexCount());
}



vkx::Image* Loader::LoadImage(const BitmapImage& img)
{
    return vkx::CreateStagedImage(img.width(), img.height(), img.pixels());
}


/*
vkx::Image* Loader::loadCubeMap(const BitmapImage* imgs)
{
    int w = imgs[0].width();
    int h = imgs[0].height();
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    int singleImageSize = w*h*4;
    char* pixels = new char[6 * singleImageSize];
    for (int i = 0; i < 6; ++i) {
        assert(imgs[i].width() == w && imgs[i].height() == h);
        std::memcpy(&pixels[i*singleImageSize], imgs[i].pixels(), singleImageSize);
    }

    vkx::CreateStagedCubemapImage(w, h, pixels, &image, &imageMemory, &imageView);

    return new vkx::Image(image, imageMemory, imageView, w, h);
}


vkx::Image* Loader::loadCubeMap_6(const std::string& filename_pattern, const char** patterns)
{
    BitmapImage imgs[6] = {
            Loader::loadPNG(Strings::fmt(filename_pattern, patterns[0])),  // todo use forloop.
            Loader::loadPNG(Strings::fmt(filename_pattern, patterns[1])),
            Loader::loadPNG(Strings::fmt(filename_pattern, patterns[2])),
            Loader::loadPNG(Strings::fmt(filename_pattern, patterns[3])),
            Loader::loadPNG(Strings::fmt(filename_pattern, patterns[4])),
            Loader::loadPNG(Strings::fmt(filename_pattern, patterns[5]))
    };
    return Loader::loadCubeMap(imgs);
}

vkx::Image* Loader::loadCubeMap_3x2(const std::string& filename)
{
    BitmapImage comp = Loader::loadPNG(filename);
    int gsize = comp.height() / 2;
    assert(std::abs(comp.width() - gsize*3) <= 1 && "expect 3x2 grid image.");

    BitmapImage imgs[6] = {{gsize, gsize}, {gsize, gsize}, {gsize, gsize},
                           {gsize, gsize}, {gsize, gsize}, {gsize, gsize}};

    glm::vec2 coords[6] = {
            {2, 1},  // +X Right
            {0, 1},  // -X Left
            {1, 0},  // +Y Top
            {0, 0},  // -Y Bottom
            {1, 1},  // +Z Front
            {2, 0}   // -Z Back
    };

    for (int i = 0; i < 6; ++i) {
        BitmapImage::CopyPixels(coords[i].x*gsize, coords[i].y*gsize, comp,
                                0, 0, imgs[i],
                                gsize, gsize);
    }

    return Loader::loadCubeMap(imgs);
}


/*


// for OpenGL 3.
//VertexArrays* Loader::loadVertexBuffer(size_t vcount, std::initializer_list<int> sizes,
//                                       float* vtx_data, size_t vtx_size, uint32_t* idx_data)
//{
//    assert(sizes.size() > 0);
//
//    VertexArrays* vao = new VertexArrays();
//    vao->vertexCount = vcount;
//    glGenVertexArrays(1, &vao->vaoId);
//    glBindVertexArray(vao->vaoId);
//
//    int _scalars = 0;
//    for (int s : sizes) { _scalars += s; }
//    int stride = _scalars * sizeof(float);
//
//    if (idx_data)
//    {
//        glGenBuffers(1, &vao->eboId);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->eboId);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * vcount, idx_data, GL_STATIC_DRAW);
//    } else {
//        vtx_size = stride*vcount;
//    }
//
//    glGenBuffers(1, &vao->vboId);
//    glBindBuffer(GL_ARRAY_BUFFER, vao->vboId);
//    glBufferData(GL_ARRAY_BUFFER, vtx_size, vtx_data, GL_STATIC_DRAW);
//
//    int i = 0;
//    _scalars = 0;
//    for (int s : sizes) {
//        glVertexAttribPointer(i, s, GL_FLOAT, GL_FALSE, stride, (void*)(_scalars*sizeof(float)));
//        glEnableVertexAttribArray(i);
//        ++i;
//        _scalars += s;
//    }
//    return vao;
//}
//
//VertexArrays* Loader::loadVertexBuffer(const VertexData* vtx)
//{
//    return Loader::loadVertexBuffer(vtx->vertexCount(), {3,2,3}, (float*)vtx->data(), vtx->size(), (uint32_t*)vtx->idx_data());
//}


// GL4.5
class VertexArray
{
public:
    GLuint vaoId = 0;
    GLuint vboId = 0;
    GLuint iboId = 0;  // if 0 means non-indexed
    uint32_t vertexCount = 0;

    VertexArray(GLuint vaoId, GLuint vboId, GLuint iboId, uint32_t vertexCount) :
                vaoId(vaoId), vboId(vboId), iboId(iboId), vertexCount(vertexCount) {}

    bool indexed() const { return iboId != 0; }
};

// no-idx:  load(vc, {3,2,3}, vtx);
// idx:     load(vc, {3,2,3}, vtx_data, vtx_size, idx_data);
VertexArray* loadVertexData(uint32_t vertexCount, std::initializer_list<int> attrib_sizes,
                    float* vtx_data, uint32_t vtx_size = -1, uint32_t* idx_data = nullptr)
{
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
    glVertexArrayVertexBuffer(vaoId, 0, vboId, 0, stride);

    int offset = 0;
    int attrib  = 0;
    for (int attrib_size : attrib_sizes)
    {
        glEnableVertexArrayAttrib(vaoId, attrib);

        glVertexArrayAttribFormat(vaoId, attrib, attrib_size, GL_FLOAT, GL_FALSE, offset);

        glVertexArrayAttribBinding(vaoId, attrib, 0);

        offset += attrib_size;
        ++attrib;
    }

    return new VertexArray(vaoId, vboId, iboId, vertexCount);
}



*/



//GLuint loadTexture(const BitmapImage& img)
//{
//    GLuint texId;
//    glCreateTextures(GL_TEXTURE_2D, 1, &texId);
//
//    glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//    int width = img.width();
//    int height = img.height();
//    void* pixels = img.pixels();
//    glTextureStorage2D(texId, 1, GL_RGBA8, width, height);
//    glTextureSubImage2D(texId, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//
//    // glGenerateTextureMipmap(texId);
//}
//
//GLuint loadCubeMap(const BitmapImage* imgs)
//{
//    GLuint texId;
//    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texId);
//
//    glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_REPEAT );
//    glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_REPEAT );
//    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//    int width = imgs[0].width();
//    int height = imgs[0].height();
//    glTextureStorage2D(texId, 1, GL_RGBA8, width, height);
//
//    for (size_t face = 0; face < 6; ++face) {
//        assert(imgs[face].width() == width && imgs[face].height() == height);
//
//        glTextureSubImage3D(texId, 0, 0, 0, face, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, imgs[face].pixels());
//    }
//
//    // glGenerateTextureMipmap(texId);
//}


//Texture* Loader::loadTexture(const BitmapImage& img)
//{
//    std::unique_ptr<std::uint32_t> pixels(new uint32_t[img.getWidth() * img.getHeight()]);
//    img.getVerticalFlippedPixels(pixels.get());
//
//    return Loader::loadTexture(img.getWidth(), img.getHeight(), pixels.get());
//}
//
//Texture* Loader::loadTexture(int w, int h, void* pixels_VertFlip, int intlfmt, int fmt, int type)
//{
//    auto* tex = Texture::GenTexture(w,h, GL_TEXTURE_2D);
//    tex->BindTexture();
//
//    glTexImage2D(GL_TEXTURE_2D, 0, intlfmt, w, h, 0, fmt, type, pixels_VertFlip);
//    // glTexSubImage2D();
//
//
//    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.2f);
////        if (GL.getCapabilities().GL_EXT_texture_filter_anisotropic) {
////            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0);  // set 0 if use TextureFilterAnisotropic
////            float amount = Math.min(4f, glGetFloat(EXTTextureFilterAnisotropic.GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT));
////            glTexParameterf(target, EXTTextureFilterAnisotropic.GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
////            LOGGER.info("ENABLED GL_EXT_texture_filter_anisotropic");
////         }
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST
//
//    glGenerateMipmap(GL_TEXTURE_2D);
//
//    return tex;
//}


//Texture* Loader::loadCubeMap_3x2(const std::string &filepath) {
//    BitmapImage comp = Loader::loadPNG(filepath);
//    int size = comp.getHeight() / 2;
//    assert(std::abs(comp.getWidth() - size*3) < 3 && "Expect 3x2 grid image.");
//
//    BitmapImage imgs[] = {{size,size},{size,size},{size,size},
//                          {size,size},{size,size},{size,size}};
//
//    comp.get_pixels_to(2*size, size, imgs[0]);  // +X Right
//    comp.get_pixels_to(0, size, imgs[1]);  // -X Left
//    comp.get_pixels_to(size, 0, imgs[2]);  // +Y Top
//    comp.get_pixels_to(0, 0, imgs[3]);  // -Y Bottom
//    comp.get_pixels_to(size, size, imgs[4]);  // +Z Front (GL)
//    comp.get_pixels_to(2*size, 0, imgs[5]);  // -Z Back (GL)
//
//    return loadCubeMap(imgs);
//}
//
//Texture* Loader::loadCubeMap(const BitmapImage* imgs)  {
//    int w = imgs[0].getWidth();
//    int h = imgs[0].getHeight();
//
//    Texture* tex = Texture::GenTexture(w,h,GL_TEXTURE_CUBE_MAP);
//    tex->BindTexture();
//
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//    for (int i = 0; i < 6; ++i) {
//        const BitmapImage& img = imgs[i];
//        assert(img.getWidth() == w && img.getHeight() == h);
//
//        // flipped y.
//        void* pixels = img.getPixels();
//
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//                     0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//    }
//
//    return tex;
//}


#pragma endregion


#pragma region OGG, WAV

/*



#include <stb/stb_vorbis.c>


int16_t* Loader::loadOGG(const DataBlock& data, size_t* dst_len, int* dst_channels, int* dst_sampleRate) {
    int channels, sample_rate;
    int16_t* pcm = nullptr;
    int len = stb_vorbis_decode_memory((unsigned char*)data.data(), data.size(), &channels, &sample_rate, &pcm);
    if (len == -1 || pcm == nullptr)
        throw std::runtime_error("failed to decode ogg.");

    // Log::info("Load ogg, pcm size {}, freq {}, cnls {}", Strings::size_str(len), sample_rate, channels);
    assert(channels == 2 || channels == 1);

    *dst_channels = channels;
    *dst_len = len;
    *dst_sampleRate = sample_rate;

    return pcm;
}

AudioBuffer* Loader::loadOGG(const DataBlock& data) {
    size_t len;
    int channels;
    int sampleRate = 0;

    int16_t* pcm = Loader::loadOGG(data, &len, &channels, &sampleRate);
    ALuint format = channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

    AudioBuffer* buf = new AudioBuffer();
    buf->buffer_data(format, pcm, len, sampleRate);
    return buf;
}


void Loader::saveWAV(std::ostream& out, const void* pcm, size_t size, int samplePerSec)
{
    // endianness problem. may cause wrong on big-endian system.
    struct WAV_HEADER {
        // RIFF Chunk Descriptor
        uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
        uint32_t ChunkSize;                     // RIFF Chunk Size
        uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
        // "fmt" sub-chunk
        uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
        uint32_t Subchunk1Size = 16;           // Size of the fmt chunk
        uint16_t AudioFormat = 1; // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
        // Mu-Law, 258=IBM A-Law, 259=ADPCM
        uint16_t NumOfChan = 1;   // Number of channels 1=Mono 2=Sterio
        uint32_t SamplesPerSec = 16000;   // Sampling Frequency in Hz
        uint32_t bytesPerSec = 16000 * 2; // bytes per second
        uint16_t blockAlign = 2;          // 2=16-bit mono, 4=16-bit stereo
        uint16_t bitsPerSample = 16;      // Number of bits per sample
        // "data" sub-chunk
        uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
        uint32_t Subchunk2Size;                        // Sampled data length
    };
    static_assert(sizeof(WAV_HEADER) == 44);

    WAV_HEADER hdr;
    hdr.ChunkSize = size + sizeof(WAV_HEADER) - 8;
    hdr.Subchunk2Size = size;

    hdr.SamplesPerSec = samplePerSec;
    hdr.bytesPerSec = samplePerSec * 2;  // 16bit sample.

    out.write(reinterpret_cast<const char*>(&hdr), sizeof(WAV_HEADER));

    out.write((char*)pcm, size);
}

*/
#pragma endregion


#pragma region FileDialogs, OS


#include <tinyfiledialogs.h>


void Loader::ShowMessageBox(const char* title, const char* message, const char* dialogType, const char* iconType, int defaultButton)
{
    tinyfd_messageBox(title, message, dialogType, iconType, defaultButton);
}

const char* Loader::ShowInputBox(const char* title, const char* message, const char* defaultInput)
{
    return tinyfd_inputBox(title, message, defaultInput);  // free()?
}

const char* Loader::OpenFileDialog(
        const char* title,
        const char* defaultPath,
        std::initializer_list<const char*> filterPatterns,
        const char* filterDesc,
        bool allowMultipleSelects)
{
    return tinyfd_openFileDialog(title, defaultPath, filterPatterns.size(), filterPatterns.begin(), filterDesc, allowMultipleSelects);
}


const char* Loader::SaveFileDialog(
    const char* title,
    const char* defaultPath,
    std::initializer_list<const char*> filterPatterns,
    const char* singleFilterDesc)
{
    return tinyfd_saveFileDialog(title, defaultPath, filterPatterns.size(), filterPatterns.begin(), singleFilterDesc);
}

const char* Loader::OpenFolderDialog(const char* title, const char* defaultPath)
{
    return tinyfd_selectFolderDialog(title, defaultPath);
}

glm::vec3 Loader::OpenColorPicker(
        const char* title,
        glm::vec3 defaultColor,
        const char* defaultHexRGB,
        const char** out_HexRGB)
{
    uint8_t rgb[3] = { defaultColor.x * 255, defaultColor.y * 255, defaultColor.z * 255 };

    const char* hexResult = tinyfd_colorChooser(title, defaultHexRGB, rgb, rgb);
    if (out_HexRGB) {
        *out_HexRGB = hexResult;
    }
    return { rgb[0] / 255.0f, rgb[1] / 255.0f, rgb[2] / 255.0f };
}


void Loader::OpenURL(std::string_view url)
{
#if _WIN32  
    std::string win_url(url);

    // replace '/' to '\'
    for (char& c : win_url)
    {
        if (c == '/') c = '\\';
    }

    std::system(("start " + win_url).c_str());
    return;
#elif __APPLE__
    std::system((std::string("open ") + url).c_str());
#elif __unix__
    std::system((std::string("xdg-open ") + url).c_str());
#else
    static_assert(false);  // Not supported OS yet.
#endif
}






// https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
std::string Loader::os_arch()
{
#if defined(__APPLE__) && defined(__x86_64__)
    return "darwin-x64";
#elif defined(__APPLE__) && defined(__aarch64__)
    return "darwin-arm64";
#elif defined(_WIN32) && (defined(__x86_64__) || defined(_M_X64))
    return "windows-x64";
#elif defined(_WIN32) && (defined(__aarch64__) || defined(_M_ARM64))
    return "windows-arm64";
#elif defined(__linux) && (defined(__amd64))
    return "linux-x64";
#elif defined(__linux) && defined(__aarch64__)
    return "linux-arm64";
#else
        static_assert(false);
#endif
}



#ifdef __APPLE__
#include <cpuid.h>
#endif

#ifdef _WIN32
#include <Windows.h>  // get system info
#endif

const char* Loader::cpuid()
{
    static char CPUBrandString[0x40]{};
    if (CPUBrandString[0])
        return CPUBrandString;

#if defined(_WIN32)

    int CPUInfo[4] = { -1 };
    unsigned   nExIds, i = 0;

    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }

    return CPUBrandString;
    //string includes manufacturer, model and clockspeed

    //SYSTEM_INFO sysInfo;
    //GetSystemInfo(&sysInfo);
    //cout << "Number of Cores: " << sysInfo.dwNumberOfProcessors << endl;
    //
    //MEMORYSTATUSEX statex;
    //statex.dwLength = sizeof(statex);
    //GlobalMemoryStatusEx(&statex);
    //cout << "Total System Memory: " << (statex.ullTotalPhys / 1024) / 1024 << "MB" << endl;

#elif defined(__APPLE__)

    unsigned int CPUInfo[4] = { 0,0,0,0 };

    __cpuid(0x80000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    unsigned int nExIds = CPUInfo[0];

    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    for (unsigned int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(i, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);

        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    return CPUBrandString;
#else
    static_assert(false);
#endif
}


/*

//const char* Loader::sysname()
//{
//#if _WIN32
//    return "windows";
//#elif __APPLE__
//    return "darwin";
//#elif __linux
//    return "linux";
//#else
//    static_assert(false);
//#endif
//}

//std::string Loader::sys_libname(const std::string& name)  {
//#if __APPLE__
//    return "lib" + name + ".dylib";
//#elif _WIN32
//    return "lib" + name + ".dll";
//#elif __linux
//    return "lib" + name + ".so";
//#else
//        static_assert(false);
//#endif
//}


*/

#pragma endregion














//#include <dj-fft/dj_fft.h>
//
//std::vector<std::complex<float>> Loader::fft_1d(const std::vector<std::complex<float>>& freq)
//{
//    return dj::fft1d(freq, dj::fft_dir::DIR_FWD);
//}