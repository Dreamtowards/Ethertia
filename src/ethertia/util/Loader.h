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
#include <ethertia/render/Model.h>
#include <ethertia/render/ShaderProgram.h>

#include <ethertia/util/BitmapImage.h>
#include <ethertia/render/VertexBuffer.h>
#include <ethertia/audio/AudioEngine.h>

// Dup with GuiCommon.h
#define DECL_Inst(T) static T* Inst() { static T* INST = new T(); return INST; }


class Loader {

public:

    inline static std::string ASSETS = "assets/";


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



    ////////////////// FILE & ASSETS //////////////////

    static datablock loadFile(const std::string& path);

    inline static std::string fileAssets(const std::string& p) {
        return ASSETS + p;
    }

    // lead with "./" or "/": normal relative/absolute path
    // otherwise: assets file.
    inline static std::string fileResolve(const std::string& p) {
        if (p.starts_with('.') || p.starts_with('/'))
            return p;
        else
            return fileAssets(p);
    }

    static datablock loadAssets(const std::string& p) {
        return loadFile(fileAssets(p));
    }

    static bool fileExists(std::string_view path);

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
    static int16_t* loadOGG(const datablock& data, size_t* dst_len, int* dst_channels, int* dst_sampleRate);

    // load to OpenAL buffer.
    static AudioBuffer* loadOGG(const datablock& data);

    // PCM, 16-bit sample, 1-channel
    static void saveWAV(const void* pcm, size_t size, std::ostream& dst, int samplePerSec = 44100);





    //////////// PNG ////////////

    static BitmapImage loadPNG(const void* data, size_t len);

    static BitmapImage loadPNG(const datablock& m) { return loadPNG(m.data, m.length); }

    static BitmapImage loadPNG(const std::string filepath) { return loadPNG(Loader::loadFile(Loader::fileResolve(filepath))); }

    static void savePNG(const BitmapImage& img, const std::string& filename);




#ifndef LOADER_NO_OPENGL




    static void loadShaderProgram(ShaderProgram* p, const std::string& assets_p)
    {
        std::string path = Loader::fileResolve(Strings::fmt(assets_p, "gsh"));
        bool geo = Loader::fileExists(path);

        p->reloadSources(Loader::loadAssets(Strings::fmt(assets_p, "vsh")).new_string(),
                          Loader::loadAssets(Strings::fmt(assets_p, "fsh")).new_string(),
                          geo ? Loader::loadFile(path).new_string() : "");
    }


    /////////////// OpenGL VAO, VBO ///////////////

    // VertexCount, {{VertLen, VertData}}
    static VertexArrays* loadModel(size_t vcount, const std::vector<std::pair<int, float*>>& vdats);

    static VertexArrays* loadModel(const VertexBuffer* vbuf);

    static VertexArrays* loadModel(size_t vcount, std::initializer_list<std::pair<int, float*>> vdats) {
        return loadModel(vcount, std::vector(vdats));
    }

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
    static std::string sys_target_name();

    // macOS:   lib<Name>.dylib
    // Windows: lib<Name>.dll
    static std::string sys_lib_name(const std::string& name);





    static std::vector<std::complex<float>> fft_1d(const std::vector<std::complex<float>>& data);

};

#endif //ETHERTIA_LOADER_H
