//
// Created by Dreamtowards on 2023/2/1.
//

#include <ethertia/util/Loader.h>
#include <ethertia/util/OBJLoader.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>



Loader::DataBlock Loader::loadFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error(Strings::fmt("Failed to open file: ", path));
    }
    size_t filesize = (size_t)file.tellg() + 1;  // +1: we add '\0' at the end. for string load.
    char* data = new char[filesize];
    data[filesize-1] = '\0';

    file.seekg(0);
    file.read(data, filesize);
    file.close();

    return Loader::DataBlock(data, filesize, path);
}


bool Loader::fileExists(std::string_view path) {
    return std::filesystem::exists(path);
}

const std::string & Loader::fileMkdirs(const std::string &filename)
{
    // mkdirs for parents of the file.
    int _dir = filename.rfind('/');
    if (_dir != std::string::npos) {
        std::filesystem::create_directories(filename.substr(0, _dir));
    }
    return filename;
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

const void* Loader::DataBlock::data() const {
    return m_Data;
}
size_t Loader::DataBlock::size() const {
    return m_Size;
}




VertexData::VertexData(const std::string& _filename) : m_Filename(_filename)
{
    Log::info("New VertexData: {}", m_Filename);
}
VertexData::~VertexData()
{
    Log::info("Delete VertexData: {} with {} vertices", m_Filename, m_Vertices.size());
}









BitmapImage Loader::loadPNG(const void* data, size_t len)
{
    int width, height, channels;
    void* pixels = stbi_load_from_memory((unsigned char*)data, len, &width, &height, &channels, 4);

    return BitmapImage(width, height, (unsigned int*)pixels);
}

void Loader::savePNG(const BitmapImage& img, const std::string& filename)
{
    Loader::fileMkdirs(filename);
    if (!stbi_write_png(filename.c_str(), img.getWidth(), img.getHeight(), 4, img.getPixels(), 0)) {
        throw std::runtime_error("Failed to write PNG. "+filename);
    }
}


//// @s
//VertexArrays* loadVertexBuffer(size_t vcount, const void* vertices_data, const std::vector<int>& strides, const void* indices_data)
//{
//    GLuint vaoId;
//    glGenVertexArrays(1, &vaoId);
//    glBindVertexArray(vaoId);
//
//    size_t vertices_data_size = 0;
//    for (int i = 0; i < ; ++i) {
//
//    }
//
//    GLuint vboId;
//    glGenBuffers(1, &vboId);
//    glBindBuffer(GL_ARRAY_BUFFER, vboId);
//    glBufferData(GL_ARRAY_BUFFER, vertices_data_size, vertices_data, GL_STATIC_DRAW);
//
//
//     glVertexAttribPointer(i, vlen, GL_FLOAT, false, 0, nullptr);
//    // glEnableVertexAttribArray(0);  // default enabled.
//}

VertexArrays* Loader::loadModel(size_t vcount, const std::vector<std::pair<int, float *>> &vdats)
{
    VertexArrays* vao = VertexArrays::GenVertexArrays();
    vao->BindVertexArrays();
    vao->vertexCount = vcount;

    int i = 0;
    for (auto vd : vdats) {
        int vlen = vd.first;
        float* vdat = vd.second;

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vlen*vcount, vdat, GL_STATIC_DRAW);

        glVertexAttribPointer(i, vlen, GL_FLOAT, false, 0, nullptr);
        glEnableVertexAttribArray(i);
        vao->vboId[i] = vbo;
        i++;
    }
    return vao;
}

VertexArrays* Loader::loadModel(const VertexBuffer* vbuf)
{
    std::vector<std::pair<int, float*>> ls;
    ls.emplace_back(3, (float*)vbuf->positions.data());
    ls.emplace_back(2, (float*)vbuf->textureCoords.data());
    ls.emplace_back(3, (float*)vbuf->normals.data());

    return loadModel(vbuf->vertexCount(), ls);
}

Texture* Loader::loadTexture(const BitmapImage& img)
{
    std::unique_ptr<std::uint32_t> pixels(new uint32_t[img.getWidth() * img.getHeight()]);
    img.getVerticalFlippedPixels(pixels.get());

    return Loader::loadTexture(img.getWidth(), img.getHeight(), pixels.get());
}

Texture* Loader::loadTexture(int w, int h, void* pixels_VertFlip, int intlfmt, int fmt, int type)
{
    auto* tex = Texture::GenTexture(w,h, GL_TEXTURE_2D);
    tex->BindTexture();

    glTexImage2D(GL_TEXTURE_2D, 0, intlfmt, w, h, 0, fmt, type, pixels_VertFlip);
    // glTexSubImage2D();


    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.2f);
//        if (GL.getCapabilities().GL_EXT_texture_filter_anisotropic) {
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0);  // set 0 if use TextureFilterAnisotropic
//            float amount = Math.min(4f, glGetFloat(EXTTextureFilterAnisotropic.GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT));
//            glTexParameterf(target, EXTTextureFilterAnisotropic.GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
//            LOGGER.info("ENABLED GL_EXT_texture_filter_anisotropic");
//         }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST

    glGenerateMipmap(GL_TEXTURE_2D);

    return tex;
}


Texture* Loader::loadCubeMap_3x2(const std::string &filepath) {
    BitmapImage comp = Loader::loadPNG(filepath);
    int size = comp.getHeight() / 2;
    assert(std::abs(comp.getWidth() - size*3) < 3 && "Expect 3x2 grid image.");

    BitmapImage imgs[] = {{size,size},{size,size},{size,size},
                          {size,size},{size,size},{size,size}};

    comp.get_pixels_to(2*size, size, imgs[0]);  // +X Right
    comp.get_pixels_to(0, size, imgs[1]);  // -X Left
    comp.get_pixels_to(size, 0, imgs[2]);  // +Y Top
    comp.get_pixels_to(0, 0, imgs[3]);  // -Y Bottom
    comp.get_pixels_to(size, size, imgs[4]);  // +Z Front (GL)
    comp.get_pixels_to(2*size, 0, imgs[5]);  // -Z Back (GL)

    return loadCubeMap(imgs);
}

Texture* Loader::loadCubeMap(const BitmapImage* imgs)  {
    int w = imgs[0].getWidth();
    int h = imgs[0].getHeight();

    Texture* tex = Texture::GenTexture(w,h,GL_TEXTURE_CUBE_MAP);
    tex->BindTexture();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    for (int i = 0; i < 6; ++i) {
        const BitmapImage& img = imgs[i];
        assert(img.getWidth() == w && img.getHeight() == h);

        // flipped y.
        void* pixels = img.getPixels();

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }

    return tex;
}




#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>


#include <nlohmann/json.hpp>


void Loader::saveOBJ(const std::string &filename, size_t verts, const float *pos, const float *uv, const float *norm)  {
    std::stringstream ss;
    OBJLoader::saveOBJ(ss, verts, pos, uv, norm);

    Loader::fileMkdirs(filename);
    std::ofstream fs(filename);
    fs << ss.str();
    fs.close();
}


// tiny_obj_loader: 2-5 times faster than util::OBJLoader. especially in little. 6.6MB obj 2times faster: 1.9s/0.9s, 632KB obj 4.3 times faster.
static void loadOBJ_Tiny(const char *filename, VertexBuffer &vbuf) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool succ = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);
    if (!succ) {
        throw std::runtime_error(Strings::fmt("failed load obj tiny: ", err));
    }

    for (tinyobj::shape_t& shape : shapes)
    {
        tinyobj::mesh_t& mesh = shape.mesh;
        size_t idx_offset = 0;

        vbuf.reserve(vbuf.vertexCount() + mesh.indices.size());

        for (int face_i = 0; face_i < mesh.num_face_vertices.size(); ++face_i)
        {
            int face_verts = mesh.num_face_vertices[face_i];
            for (int fvi = 0; fvi < face_verts; ++fvi)
            {
                tinyobj::index_t idx = mesh.indices[idx_offset + fvi];

                int ip = 3*idx.vertex_index;
                tinyobj::real_t px = attrib.vertices[ip],
                        py = attrib.vertices[ip+1],
                        pz = attrib.vertices[ip+2];
                vbuf.addpos(px,py,pz);

                if (idx.normal_index >= 0) {
                    int in = 3*idx.normal_index;
                    tinyobj::real_t nx = attrib.normals[in],
                            ny = attrib.normals[in+1],
                            nz = attrib.normals[in+2];
                    vbuf.addnorm(nx,ny,nz);
                }

                if (idx.texcoord_index >= 0) {
                    int it = 2*idx.texcoord_index;
                    tinyobj::real_t tx = attrib.texcoords[it],
                            ty = attrib.texcoords[it+1];
                    vbuf.adduv(tx,ty);
                }
            }
            idx_offset += face_verts;

            // mesh.material_ids[face_i];
        }
    }
}

VertexBuffer* Loader::loadOBJ(const std::string& filepath)  {
    VertexBuffer* vbuf = new VertexBuffer();
    loadOBJ_Tiny(Loader::fileResolve(filepath).c_str(), *vbuf);
    return vbuf;
}


#include <stb/stb_vorbis.c>


int16_t* Loader::loadOGG(const DataBlock& data, size_t* dst_len, int* dst_channels, int* dst_sampleRate) {
    int channels = 0;
    int sample_rate = 0;
    int16_t* pcm = nullptr;
    int len = stb_vorbis_decode_memory((unsigned char*)data.data(), data.size(), &channels, &sample_rate, &pcm);
    if (len == -1)
        throw std::runtime_error("failed decode ogg.");
    assert(pcm);

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


#include <dj-fft/dj_fft.h>

std::vector<std::complex<float>> Loader::fft_1d(const std::vector<std::complex<float>>& freq)
{
    return dj::fft1d(freq, dj::fft_dir::DIR_FWD);
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



const char* Loader::sysname()
{
#if _WIN32
    return "WINDOWS";
#elif __APPLE__
    return "DARWIN";
#elif __unix__
    return "LINUX";
#else
        static_assert(false);
#endif
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
#else
        static_assert(false);
#endif
}

std::string Loader::sys_libname(const std::string& name)  {
#if __APPLE__
    return "lib" + name + ".dylib";
#elif _WIN32
    return "lib" + name + ".dll";
#elif __linux
    return "lib" + name + ".so";
#else
        static_assert(false);
#endif
}


size_t Loader::calcDirectorySize(const std::string& dir)
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









void Loader::saveWAV(const void* pcm, size_t size, std::ostream& dst, int samplePerSec)
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

    dst.write(reinterpret_cast<const char*>(&hdr), sizeof(WAV_HEADER));

    dst.write((char*)pcm, size);
}