//
// Created by Dreamtowards on 2023/2/1.
//

#include "Loader.h"


// #define DATA_INFO 1



Loader::DataBlock Loader::loadFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error(Strings::fmt("Failed to open file: ", path));
    }
    size_t filesize = (size_t)file.tellg();
    char* data = new char[filesize + 1];  // +1: add an extra '\0' at the end. for c_string format compatible.
    data[filesize] = '\0';  // set last extra byte to '\0'.

    file.seekg(0);
    file.read(data, filesize);
    file.close();

    return Loader::DataBlock(data, filesize, path);
}


bool Loader::fileExists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

#include <ethertia/init/Settings.h>

std::string Loader::fileAssets(const std::string& p)
{
    for (const std::string& basepath : Settings::ASSETS)
    {
        std::string path = basepath + p;
        if (Loader::fileExists(path))
            return path;
    }
    return "";
    //throw std::runtime_error(Strings::fmt("failed to locate assets file: {}", p));
}

std::string Loader::fileResolve(const std::string& p)
{
    if (p.starts_with('.') || p.starts_with('/') || (p.find(':') == 1))
        return p;
    else
        return fileAssets(p);
}

Loader::DataBlock Loader::loadAssets(const std::string& p)
{
    return loadFile(fileAssets(p));
}



const std::string& Loader::fileMkdirs(const std::string& filename)
{
    // mkdirs for parents of the file.
    int _dir = filename.rfind('/');
    if (_dir != std::string::npos) {
        std::filesystem::create_directories(filename.substr(0, _dir));
    }
    return filename;
}



size_t Loader::dirSize(const std::string& dir)
{
    size_t sumSize = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
    {
        if (!entry.is_directory()) {
            sumSize += entry.file_size();
        }
    }
    return sumSize;
}








Loader::DataBlock::DataBlock(void* data, size_t size, const std::string& filename)
        : m_Data(data), m_Size(size), m_Filename(filename)
{
#ifdef DATA_INFO
    Log::info("New DataBlock: {} of {} bytes", m_Filename, size);
#endif
}

Loader::DataBlock::~DataBlock()
{
    std::free(m_Data);
#ifdef DATA_INFO
    Log::info("Delete DataBlock: {}", m_Filename);
#endif
}

const void* Loader::DataBlock::data() const {
    return m_Data;
}
size_t Loader::DataBlock::size() const {
    return m_Size;
}









#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

#include <unordered_map>


// Arrays vs Indexed compare for viking_room.obj (Single Vertex is vec3+vec2+vec3 8*f32 = 32 bytes, Index is uint32 = 4 bytes)
// (arrays: 11484 vert *32 = 367,488 bytes) dbg-run 18ms
// (indexed: 4725 unique vert *32 = 151,200 bytes (=x0.41) + 11484 indices * 4 = 45,936 bytes  =  197,136 bytes (=x0.54)) dbg-run 21ms

VertexData* Loader::loadOBJ_(const char* filename)
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
        vtx->m_Vertices.reserve(vtx->m_Vertices.size() + shape.mesh.indices.size());

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
                unique_verts[vert] = vtx->m_Vertices.size();
                vtx->m_Vertices.push_back(vert);
            }
            vtx->m_Indices.push_back(unique_verts[vert]);
        }
    }
//#ifdef DATA_INFO
    Log::info("Load OBJ {} of {} vertexCount, {} unique vertices ({}% +{}% idx)",
              filename, vtx->vertexCount(), vtx->m_Vertices.size(),
              (float)vtx->m_Vertices.size()/vtx->vertexCount(),
              (float)vtx->m_Indices.size()/vtx->vertexCount()/8.0f);
//#endif
    return vtx;
}



#include <ethertia/util/OBJLoader.h>

void Loader::saveOBJ(const std::string& filename, size_t verts, const float* pos, const float* uv, const float* norm)
{
    std::stringstream ss;
    OBJLoader::saveOBJ(ss, verts, pos, uv, norm);

    Loader::fileMkdirs(filename);
    std::ofstream fs(filename);
    fs << ss.str();
    fs.close();
}











#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>




BitmapImage Loader::loadPNG_(const char* filename)
{
    int w, h, channels;
    void* pixels = stbi_load(filename, &w, &h, &channels, 4);

    return BitmapImage(w, h, pixels);
}

//BitmapImage Loader::loadPNG(const void* data, size_t len)
//{
//    int w, h, channels;
//    void* pixels = stbi_load_from_memory((unsigned char*)data, len, &w, &h, &channels, 4);
//
//    return BitmapImage(w, h, pixels);
//}

void Loader::savePNG(const std::string& filename, const BitmapImage& img)
{
    Loader::fileMkdirs(filename);
    if (!stbi_write_png(filename.c_str(), img.getWidth(), img.getHeight(), 4, img.getPixels(), 0)) {
        throw std::runtime_error("Failed to write PNG. "+filename);
    }
}

















// for OpenGL.
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




vkx::VertexBuffer* Loader::loadVertexBuffer(const VertexData* vtx)
{
    VkBuffer vtxBuffer;
    VkDeviceMemory vtxMemory;
    vkx::CreateStagedBuffer(vtx->vtx_data(), vtx->vtx_size(), &vtxBuffer, &vtxMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    VkBuffer idxBuffer = nullptr;
    VkDeviceMemory idxMemory = nullptr;
    if (vtx->isIndexed())
    {
        vkx::CreateStagedBuffer(vtx->idx_data(), vtx->idx_size(), &idxBuffer, &idxMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    }

    int vcount = vtx->vertexCount();
    return new vkx::VertexBuffer(vtxBuffer, vtxMemory, idxBuffer, idxMemory, vcount);
}




vkx::Image* Loader::loadImage(const BitmapImage& img)
{
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    vkx::CreateStagedImage(img.getWidth(), img.getHeight(), img.getPixels(),
                           &image, &imageMemory, &imageView);

    return new vkx::Image(image, imageMemory, imageView, img.getWidth(), img.getHeight());
}









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
        /* RIFF Chunk Descriptor */
        uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
        uint32_t ChunkSize;                     // RIFF Chunk Size
        uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
        /* "fmt" sub-chunk */
        uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
        uint32_t Subchunk1Size = 16;           // Size of the fmt chunk
        uint16_t AudioFormat = 1; // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
        // Mu-Law, 258=IBM A-Law, 259=ADPCM
        uint16_t NumOfChan = 1;   // Number of channels 1=Mono 2=Sterio
        uint32_t SamplesPerSec = 16000;   // Sampling Frequency in Hz
        uint32_t bytesPerSec = 16000 * 2; // bytes per second
        uint16_t blockAlign = 2;          // 2=16-bit mono, 4=16-bit stereo
        uint16_t bitsPerSample = 16;      // Number of bits per sample
        /* "data" sub-chunk */
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













#include <tinyfd/tinyfiledialogs.h>


void Loader::showMessageBox(const char* title, const char* message)  {
    tinyfd_messageBox(title, message, "ok", "question", 1);
}

const char* Loader::showInputBox(const char* title, const char* message, const char* def) {
    return tinyfd_inputBox(title, message, def);  // free()?
}

glm::vec3 Loader::openColorPick()  {
    uint8_t rgb[3] = {};
    tinyfd_colorChooser("Color Choose", nullptr, rgb, rgb);
    return {rgb[0] / 255.0f, rgb[1] / 255.0f, rgb[2] / 255.0f};
}


const char* Loader::openFileDialog(const char* title,
                            const char* defpath,
                            std::initializer_list<const char*> filepatterns,
                            const char* desc,
                            bool allowMultipleFiles)
{
    return tinyfd_openFileDialog(title, defpath, filepatterns.size(), filepatterns.begin(), desc, allowMultipleFiles);
}

const char* Loader::openFolderDialog(const char* title, const char* defpath) {
    return tinyfd_selectFolderDialog(title, defpath);
}



void Loader::openURL(const std::string &url)
{
    const char* cmd = nullptr;
#if _WIN32
    cmd = "start ";  // windows
#elif __APPLE__
    cmd = "open ";  // macos
#elif __unix__
    cmd = "xdg-open ";  // linux
#else
    static_assert(false);  // Not supported OS yet.
#endif
    std::system(std::string(cmd + url).c_str());
}



// https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
std::string Loader::sys_target()
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
















#include <dj-fft/dj_fft.h>

std::vector<std::complex<float>> Loader::fft_1d(const std::vector<std::complex<float>>& freq)
{
    return dj::fft1d(freq, dj::fft_dir::DIR_FWD);
}
