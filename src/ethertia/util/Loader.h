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
#include <complex>

#include <nbt/nbt_tags.h>
#include <glad/glad.h>

#include <ethertia/render/Texture.h>
#include <ethertia/render/VertexArrays.h>
#include <ethertia/render/ShaderProgram.h>

#include <ethertia/util/BitmapImage.h>
#include <ethertia/render/VertexBuffer.h>
#include <ethertia/audio/AudioEngine.h>

// Dup with GuiCommon.h
#define DECL_Inst(T) static T* Inst() { static T* INST = new T(); return INST; }

// Assets Location
class AssetId
{
public:
    AssetId(const std::string& uri) : m_URI(uri) {}

private:
    std::string m_URI = nullptr;

//    operator std::string() {
//        return
//    }
};

class VertexData
{
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 tex;
        glm::vec3 norm;
    };

    VertexData(const std::string& _filename);
    ~VertexData();

    const void* data() const { return m_Vertices.data(); }
    size_t size() const { return sizeof(m_Vertices[0]) * m_Vertices.size(); }

    size_t vertexCount() const { return m_Vertices.size(); }

    std::vector<Vertex> m_Vertices;
    std::string m_Filename;  // optional. debug tag.
};


class Loader {

public:
    struct DataBlock
    {
        const void* data() const;
        size_t size() const;

        DataBlock(void* data, size_t size, const std::string& filename);
        ~DataBlock();

    private:
        void*       m_Data;
        size_t      m_Size;
        std::string m_Filename;  // optional. debug tag.
    };



    ////////////////// FILE & ASSETS //////////////////


    // load entire file. filename could be absolute-path or relative-path.
    static DataBlock loadFile(const std::string& filename);


    // produce real filename by resolve asset-path.
    static std::string fileAssets(const std::string& p);

    // lead with "./" or "/": normal relative/absolute path
    // otherwise: assets file.
    inline static std::string fileResolve(const std::string& p) {
        if (p.starts_with('.') || p.starts_with('/'))
            return p;
        else
            return fileAssets(p);
    }

    static DataBlock loadAssets(const std::string& p) {
        return loadFile(fileAssets(p));
    }

    static bool fileExists(const std::filesystem::path& path);

    // mkdirs for the file's parent dir
    static const std::string& fileMkdirs(const std::string& filename);

    // Recursive check all file size.
    static size_t calcDirectorySize(const std::string& dir);

    static void checkWorkingDirectory() {
        if (!fileExists("./assets")) {
            throw std::runtime_error("default assets directory not found. make sure you are in valid working directory.");
        }
    }



    ///////////////// OBJ /////////////////

    // arg: filepath
    static VertexBuffer* loadOBJ(const std::string& filename);

    static void saveOBJ(const std::string& filename, size_t verts, const float* pos, const float* uv =nullptr, const float* norm =nullptr);



    ////////////////// SOUNDS: OGG, WAV //////////////////

    // return: PCM data, 16 bit.
    static int16_t* loadOGG(const DataBlock& data, size_t* dst_len, int* dst_channels, int* dst_sampleRate);

    // load to OpenAL buffer.
    static AudioBuffer* loadOGG(const DataBlock& data);

    // PCM, 16-bit sample, 1-channel
    static void saveWAV(const void* pcm, size_t size, std::ostream& dst, int samplePerSec = 44100);





    //////////// PNG ////////////

    static BitmapImage loadPNG(const void* data, size_t len);
    //static BitmapImage loadPNG(const char* filename);


    static BitmapImage loadPNG(const DataBlock& m) { return loadPNG(m.data(), m.size()); }

    static BitmapImage loadPNG(const std::string uri) { return loadPNG(Loader::loadFile(Loader::fileResolve(uri))); }

    static void savePNG(const BitmapImage& img, const std::string& filename);




#ifndef LOADER_NO_OPENGL




    static void loadShaderProgram(ShaderProgram* p, const std::string& uri, bool gsh = false)
    {
        p->load((char*)Loader::loadAssets(Strings::fmt(uri, "vsh")).data(),
                (char*)Loader::loadAssets(Strings::fmt(uri, "fsh")).data(),
                (char*)(gsh ? Loader::loadAssets(Strings::fmt(uri, "gsh")).data() : nullptr));
    }


    /////////////// OpenGL VAO, VBO ///////////////

    // VertexCount, {{VertLen, VertData}}
    static VertexArrays* loadModel(size_t vcount, const std::vector<std::pair<int, float*>>& vdats);

    static VertexArrays* loadModel(const VertexBuffer* vbuf);

    static VertexArrays* loadModel(size_t vcount, std::initializer_list<std::pair<int, float*>> vdats) {
        return loadModel(vcount, std::vector(vdats));
    }

    // load CPU VertexData to GPU VertexBuffer.
    // use Compact
//    static VertexArrays* loadVertexBuffer(size_t vcount, const void* data, std::vector<int> strides);

    ////////////////// OpenGL Texture: 2D, CubeMap //////////////////

    /// pixels_VertFlip: need y/vertical flipped pixels. cause of GL feature.
    static Texture* loadTexture(int w, int h, void* pixels_VertFlip,
                                int intlfmt = GL_RGBA, int fmt = GL_RGBA, int type = GL_UNSIGNED_BYTE);

    static Texture* loadTexture(const BitmapImage& img);

    static Texture* loadTexture(const std::string& filepath) {
        return Loader::loadTexture(Loader::loadPNG(filepath));
    }


    // imgs order: Right, Left, Top, Bottom, Front, Back.
    static Texture* loadCubeMap(const BitmapImage imgs[]);

    // a 3x2 grid png. first row: bottom, top, back, second row: left front, right
    static Texture* loadCubeMap_3x2(const std::string& filepath);

    // filepath: a pattern e.g. "skybox-{}.png", the {} would be replaced with "right/left/top/bottom/front/back".
    static Texture* loadCubeMap_6(const std::string& filepath) {
        BitmapImage imgs[] = {
                Loader::loadPNG(Strings::fmt(filepath, "right")),
                Loader::loadPNG(Strings::fmt(filepath, "left")),
                Loader::loadPNG(Strings::fmt(filepath, "top")),
                Loader::loadPNG(Strings::fmt(filepath, "bottom")),
                Loader::loadPNG(Strings::fmt(filepath, "front")),
                Loader::loadPNG(Strings::fmt(filepath, "back")),
        };
        return loadCubeMap(imgs);
    }


#endif




    ////////// SYSTEM //////////


    static void showMessageBox(const char* title, const char* message);

    static const char* showInputBox(const char* title, const char* message, const char* def);

    static glm::vec3 openColorPick();

    static const char* openFileDialog(const char* title = nullptr,
                               const char* defpath = nullptr,
                               std::initializer_list<const char*> filepatterns = {},
                               const char* desc = nullptr,
                               bool allowMultipleFiles = false);

    static const char* openFolderDialog(const char* title = "", const char* defpath = "");



    // File, Folder, URL
    static void openURL(const std::string& url);

    // WINDOWS / DARWIN / LINUX / nullptr
    static const char* sysname();

    // macOS:   darwin-x64  | darwin-arm64
    // Windows: windows-x64 | windows-arm64
    // Linux:   linux-x64
    static std::string sys_target();

    // deprecated: no where used. it's originally used for locating mod's binary program.
    // macOS:   lib<Name>.dylib
    // Windows: lib<Name>.dll
    // static std::string sys_libname(const std::string& name);





    static std::vector<std::complex<float>> fft_1d(const std::vector<std::complex<float>>& data);

};

#endif //ETHERTIA_LOADER_H
