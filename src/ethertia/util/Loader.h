//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <initializer_list>
#include <utility>
#include <fstream>
#include <array>
#include <filesystem>
#include <span>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <tinyfd/tinyfiledialogs.h>
#include <tiny_obj_loader/tiny_obj_loader.h>

#include <ethertia/render/Texture.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/render/VertexBuffer.h>
#include <ethertia/render/Model.h>
#include <ethertia/util/OBJLoader.h>
#include <ethertia/audio/AudioEngine.h>
#include <ethertia/render/shader/ShaderProgram.h>


class Loader {

public:

    inline static std::string ASSETS = "assets/";

    struct end_scope_dealloc {
        void* ptr;

        end_scope_dealloc(void* _ptr) : ptr(_ptr) {}

        ~end_scope_dealloc() {
            free(ptr);
        }
    };
#define AUTO_DELETE(ptr) end_scope_dealloc _s(ptr);

    struct datablock
    {
        void* data;
        size_t length;  // len in bytes

        std::string new_string() {
            return std::string((char*)data, length);
        }

        datablock(void* _data, size_t _len) : data(_data), length(_len) {}

        datablock(const datablock& cpy) {
            assert(false && "Implicit copy is disabled due to big consume.");
        }

        ~datablock() {
            free(data);
        }
    };


    static datablock loadFile(std::string_view path, bool isAssets = false)
    {
        if (isAssets)
            path = fileAssets(path);

        std::ifstream file(path, std::ios_base::binary);
        if (!file.is_open()) {
            throw std::runtime_error(Strings::fmt("Failed open file. ", path));
            //Log::warn("Failed open file: ", path);
            //return std::make_pair(nullptr, -1);
        }
        file.seekg(0, std::ios_base::end);
        size_t len = file.tellg();
        file.seekg(0, std::ios_base::beg);

        char* buf = new char[len];
        file.read(buf, len);
        file.close();

        return datablock(buf, len);
    }
    static bool fileExists(std::string_view path) {
        return std::filesystem::exists(path);
    }
    inline static std::string fileAssets(std::string_view p) {
        return ASSETS + std::string(p);
    }
    inline static std::string fileResolve(std::string_view p) {
        if (p.starts_with('@')) return fileAssets(p);
        else return std::string(p);
    }

    static datablock loadAssets(const std::string& p) {
        return loadFile(fileAssets(p));
    }

//    static std::string loadAssetsStr(const std::string& p) {
//        return loadAssets(p).new_string();
//    }
//    static std::string loadFileStr(const std::string& p) {
//        return loadFile(p).new_string();
//    }


    static ShaderProgram loadShaderProgram(const std::string& assets_p, bool geo = false)
    {
        return ShaderProgram(Loader::loadAssets(Strings::fmt(assets_p, "vsh")).new_string(),
                             Loader::loadAssets(Strings::fmt(assets_p, "fsh")).new_string(),
                             geo ? Loader::loadAssets(Strings::fmt(assets_p, "gsh")).new_string() : "");
    }


    static VertexBuffer* loadOBJ_(const char* p, bool isAssets = true) {
        VertexBuffer* vbuf = new VertexBuffer();
        loadOBJ_Tiny(isAssets ? Loader::fileAssets(p).c_str() : p, *vbuf);
        return vbuf;
//        return Loader::loadOBJ(Loader::loadFile(p, isAssets).new_string());
    }

//    static VertexBuffer* loadOBJ(const std::string& objstr) {
//        VertexBuffer* vbuf = new VertexBuffer();
//        std::stringstream ss(objstr);
//        OBJLoader::loadOBJ(ss, vbuf);
//        return vbuf;
//    }

    // tiny_obj_loader: 2-5 times faster than util::OBJLoader. especially in little. 6.6MB obj 2times faster: 1.9s/0.9s, 632KB obj 4.3 times faster.
    static void loadOBJ_Tiny(const char* filename, VertexBuffer& vbuf) {
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
                                    pz = attrib.vertices[ip+3];
                    vbuf.addpos(px,py,pz);

                    if (idx.normal_index >= 0) {
                        int in = 3*idx.normal_index;
                        tinyobj::real_t nx = attrib.normals[in],
                                        ny = attrib.normals[in+1],
                                        nz = attrib.normals[in+2];
                        vbuf.addnorm(nx,ny,nz);
                    }

                    if (idx.texcoord_index >= 0) {
                        int it = 3*idx.texcoord_index;
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

    static void saveOBJ(const std::string& filename, size_t verts, float* pos, float* uv =nullptr, float* norm =nullptr) {
        std::stringstream ss;
        OBJLoader::saveOBJ(ss, verts, pos, uv, norm);

        ensureFileParentDirsReady(filename);
        std::ofstream fs(filename);
        fs << ss.str();
        fs.close();
    }

    static int16_t* loadOGG(std::pair<char*, size_t> data, size_t* dst_len, int* dst_channels, int* dst_sampleRate);


    static AudioBuffer* loadOGG(std::pair<char*, size_t> data) {
        size_t len;
        int channels;
        int sampleRate = 0;

        int16_t* pcm = Loader::loadOGG(data, &len, &channels, &sampleRate);
        ALuint format = channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

        AudioBuffer* buf = new AudioBuffer();
        buf->buffer_data(format, pcm, len, sampleRate);
        return buf;
    }

    // PCM, 16-bit sample, 1-channel
    static void saveWAV(const void* pcm, size_t size, std::ostream& dst, int samplePerSec = 44100) {
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

    static BitmapImage loadPNG(const void* data, u32 len) {
        int width, height, channels;
        void* pixels = stbi_load_from_memory((unsigned char*)data, len, &width, &height, &channels, 4);
        return BitmapImage(width, height, (unsigned int*)pixels);
    }
    static BitmapImage loadPNG(const datablock& m) {
        return loadPNG(m.data, m.length);
    }


    static void savePNG(const BitmapImage& img, const std::string& filename) {
        ensureFileParentDirsReady(filename);
        if (!stbi_write_png(filename.c_str(), img.getWidth(), img.getHeight(), 4, img.getPixels(), 0)) {
            throw std::runtime_error("Failed to write PNG. "+filename);
        }
    }

    static void ensureFileParentDirsReady(const std::string& filename) {
        // mkdirs for parents of the file.
        int _dir = filename.rfind('/');
        if (_dir != std::string::npos) {
            std::filesystem::create_directories(filename.substr(0, _dir));
        }
    }

    static Model* loadModel(size_t vcount, const std::vector<std::pair<int, float*>>& vdats) {
        u32 vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        Model* m = new Model(vao, vcount);

        int i = 0;
        for (auto vd : vdats) {
            int vlen = vd.first;
            float* vdat = vd.second;

            u32 vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vlen*vcount, vdat, GL_STATIC_DRAW);

            glVertexAttribPointer(i, vlen, GL_FLOAT, false, 0, nullptr);
            glEnableVertexAttribArray(i);
            m->vbos.push_back(vbo);
            i++;
        }
        return m;
    }
    static Model* loadModel(VertexBuffer* vbuf) {
        std::vector<std::pair<int, float*>> ls;
        ls.emplace_back(3, &vbuf->positions[0]);
        ls.emplace_back(2, &vbuf->textureCoords[0]);
        ls.emplace_back(3, &vbuf->normals[0]);

        return loadModel(vbuf->vertexCount(), ls);
    }
    static Model* loadModel(size_t vcount, std::initializer_list<std::pair<int, float*>> vdats) {
        return loadModel(vcount, std::vector(vdats));
    }

    static Texture* loadTexture(const std::string& p, bool isAssets = true) {
        assert(isAssets || p.starts_with("./"));
        return Loader::loadTexture(Loader::loadPNG(isAssets ? Loader::loadAssets(p) : Loader::loadFile(p)));
    }
    static Texture* loadTexture(const BitmapImage& img) {
        std::unique_ptr<std::uint32_t> pixels(new std::uint32_t[img.getWidth() * img.getHeight()]);
        img.getVerticalFlippedPixels(pixels.get());

        return Loader::loadTexture(img.getWidth(), img.getHeight(), pixels.get());
    }

    /// pixels_VertInv: need y/vertical flipped pixels. cause of GL feature.
    static Texture* loadTexture(int w, int h, u32* pixels_VertFlip, int intlfmt = GL_RGBA, int fmt = GL_RGBA, int type = GL_UNSIGNED_BYTE) {
        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        auto* tex = new Texture(texId, w, h);

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

    // a 3x2 grid png. first row: bottom, top, back, second row: left front, right
    static Texture* loadCubeMap1(const std::string& p, bool isAssets = true) {
        assert(isAssets);
        BitmapImage comp = Loader::loadPNG(Loader::loadFile(p, isAssets));
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
    static Texture* loadCubeMap6(const std::string& p, bool isAssets = true) {
        assert(isAssets);

        BitmapImage imgs[] = {
                Loader::loadPNG(Loader::loadAssets(Strings::fmt(p, "right"))),
                Loader::loadPNG(Loader::loadAssets(Strings::fmt(p, "left"))),
                Loader::loadPNG(Loader::loadAssets(Strings::fmt(p, "top"))),
                Loader::loadPNG(Loader::loadAssets(Strings::fmt(p, "bottom"))),
                Loader::loadPNG(Loader::loadAssets(Strings::fmt(p, "front"))),
                Loader::loadPNG(Loader::loadAssets(Strings::fmt(p, "back"))),
        };
        return loadCubeMap(imgs);
    }

    // imgs order: Right, Left, Top, Bottom, Front, Back.
    static Texture* loadCubeMap(const BitmapImage imgs[]) {
        int w = imgs[0].getWidth();
        int h = imgs[0].getHeight();

        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
        Texture* tex = new Texture(texId, w, h);

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





    static void showMessageBox(const char* title, const char* message) {
        tinyfd_messageBox(title, message, "ok", "question", 1);
    }
    static std::string showInputBox(const char* title, const char* message, const char* def) {
        return tinyfd_inputBox(title, message, def);  // free()?
    }



    // File, Folder, URL
    static void openURL(const std::string& url) {
        const char* cmd = nullptr;
#if _WIN32
        cmd = "start ";  // windows
#elif __APPLE__
        cmd = "open ";  // macos
#elif __unix__
        cmd = "xdg-open ";  // linux
#else
        assert(false);  // Not supported OS yet.
#endif
        std::system(std::string(cmd + url).c_str());
    }

    static const char* system() {
#if __WIN32__
        return "WINDOWS";
#elif __APPLE__
        return "DARWIN";
#elif __unix__
        return "LINUX";
#else
        return "_UNKNOWN";
#endif
    }
};

#endif //ETHERTIA_LOADER_H
