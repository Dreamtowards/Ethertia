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
#include <span>

#include <ethertia/render/VertexData.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/audio/AudioEngine.h>

#include <vkx.h>


// Dup with GuiCommon.h
#define DECL_Inst(T) static T* Inst() { static T* INST = new T(); return INST; }


class Loader
{
public:


    ////////////////// FILE & ASSETS //////////////////

    struct DataBlock
    {
        const void* data() const;
        size_t size() const;

        DataBlock(void* data, size_t size, const std::string& filename);
        ~DataBlock();

        operator std::span<const char>() const;

    private:
        void*       m_Data;
        size_t      m_Size;
        std::string m_Filename;  // optional. debug tag.
    };


    // load entire file.
    static DataBlock loadFile(const std::string& filename);

    // locate a real filename of an assets-path.
    // return empty string if cannot locate the assets file.
    static std::string fileAssets(const std::string& p);

    // a quick utility func of `loadFile(fileAssets(p));`
    static DataBlock loadAssets(const std::string& p);

    // experimental.
    // locate a real filename of: an actual-path leading with './', '/', 'C:/' OR an assets-path
    static std::string fileResolve(const std::string& p);


    static bool fileExists(const std::filesystem::path& path);

    // mkdirs for the "dir/" or a file's parent dir "dir/somefile"
    static const std::string& fileMkdirs(const std::string& filename);

    // !Heavy IO Cost. recursive calc all file size.
    static size_t dirSize(const std::string& dir);






    ///////////////// OBJ /////////////////


    // internal absolute filename. load indexed unique vertices. backend: tiny_obj_loader.
    static VertexData* loadOBJ_(const char* filename);

    static VertexData* loadOBJ(const std::string& uri) { return Loader::loadOBJ_(Loader::fileResolve(uri).c_str()); }

    // quick save for debug, no compression.
    static void saveOBJ(const std::string& filename, size_t verts, const float* pos, const float* uv =nullptr, const float* norm =nullptr);






    //////////// PNG ////////////

    // todo: return BitmapImage* ptr. for more flexible

    // internal. stbi_load(filename). load file directly might optimizer than stbi_load_from_memory.
    static BitmapImage loadPNG_(const char* filename);

    static BitmapImage loadPNG(const std::string& uri) { return Loader::loadPNG_(Loader::fileResolve(uri).c_str()); }


    // stbi_load_from_memory().
    // static BitmapImage loadPNG(const void* data, size_t len);

    // helper func of load_from_memory. allows simply loadPNG(loadFile());
    // static BitmapImage loadPNG(const DataBlock& m) { return loadPNG(m.data(), m.size()); }



    static void savePNG(const std::string& filename, const BitmapImage& img);




    ////////////////// SOUNDS: OGG, WAV //////////////////


    // return: PCM data, 16 bit.
    // @out_len: num of samples of pcm data. len*sizeof(int16_t)==size_bytes.
    // @out_channels: 1 or 2. (mono or stereo)
    static int16_t* loadOGG(const DataBlock& data, size_t* out_len, int* out_channels, int* dst_sampleRate);

    // load to OpenAL buffer.
    static AudioBuffer* loadOGG(const DataBlock& data);


    // PCM, 16-bit sample, 1-channel
    static void saveWAV(std::ostream& out, const void* pcm, size_t size, int samplePerSec = 44100);














//    static void loadShaderProgram(ShaderProgram* p, const std::string& uri, bool gsh = false)
//    {
//        p->load((char*)Loader::loadAssets(Strings::fmt(uri, "vsh")).data(),
//                (char*)Loader::loadAssets(Strings::fmt(uri, "fsh")).data(),
//                (char*)(gsh ? Loader::loadAssets(Strings::fmt(uri, "gsh")).data() : nullptr));
//    }


    // load to GPU. interleaved vertex data.
    static vkx::VertexBuffer* loadVertexData(const VertexData* vtx);





    /// pixels_VertFlip: need y/vertical flipped pixels. cause of GL feature.
//    static vkx::Image* loadTexture(int w, int h, void* pixels_VertFlip,
//                                int intlfmt = GL_RGBA, int fmt = GL_RGBA, int type = GL_UNSIGNED_BYTE);


    // Image ? Texture
    static vkx::Image* loadTexture(const BitmapImage& img);

    static vkx::Image* loadTexture(const std::string& filepath) { return Loader::loadTexture(Loader::loadPNG(filepath)); }


    // @imgs 6 Images, order: +X Right, -X Left, +Y Top, -Y Bottom, +Z Front, -Z Back.
    static vkx::Image* loadCubeMap(const BitmapImage* imgs);

    // in the @filename_pattern, "{}" will be replaced with:
    inline static const char* CUBEMAP_PAT[] = {"right", "left", "top", "bottom", "front", "back"};

    // load 6 individual image.
    // @filename_pattern: e.g. "skybox-{}.png", the "{}" will be replaced with @patterns e.g. right/left/top/bottom/front/back.
    static vkx::Image* loadCubeMap_6(const std::string& filename_pattern, const char** patterns = CUBEMAP_PAT);

    // load a 3x2 grid image, first row: bottom, top, back, second row: left front, right
    static vkx::Image* loadCubeMap_3x2(const std::string& filename);


//    // imgs order: Right, Left, Top, Bottom, Front, Back.
//    static Texture* loadCubeMap(const BitmapImage imgs[]);
//
//    // a 3x2 grid png. first row: bottom, top, back, second row: left front, right
//    static Texture* loadCubeMap_3x2(const std::string& filepath);
//
//    // filepath: a pattern e.g. "skybox-{}.png", the {} would be replaced with "right/left/top/bottom/front/back".
//    static Texture* loadCubeMap_6(const std::string& filepath) {
//        BitmapImage imgs[] = {
//                Loader::loadPNG(Strings::fmt(filepath, "right")),
//                Loader::loadPNG(Strings::fmt(filepath, "left")),
//                Loader::loadPNG(Strings::fmt(filepath, "top")),
//                Loader::loadPNG(Strings::fmt(filepath, "bottom")),
//                Loader::loadPNG(Strings::fmt(filepath, "front")),
//                Loader::loadPNG(Strings::fmt(filepath, "back")),
//        };
//        return loadCubeMap(imgs);
//    }














    ///////////// OS /////////////


    static void showMessageBox(const char* title, const char* message);

    static const char* showInputBox(const char* title, const char* message, const char* def);

    static const char* openFileDialog(const char* title = nullptr,
                               const char* defpath = nullptr,
                               std::initializer_list<const char*> filepatterns = {},
                               const char* desc = nullptr,
                               bool allowMultipleFiles = false);

    static const char* openFolderDialog(const char* title = "", const char* defpath = "");

    static glm::vec3 openColorPick();



    // open File, Folder, URL
    static void openURL(const std::string& url);


    // macOS:   darwin-x64  | darwin-arm64
    // Windows: windows-x64 | windows-arm64
    // Linux:   linux-x64   | linux-arm64
    static std::string sys_target();


    // deprecated: no where used.
    // windows / darwin / linux
    // static const char* sys_name();

    // deprecated: no where used. it's originally used for locating mod's binary program.
    // macOS:   lib<Name>.dylib
    // Windows: lib<Name>.dll
    // static std::string sys_libname(const std::string& name);

    static const char* cpuid();


    ////////////// Misc //////////////


    static std::vector<std::complex<float>> fft_1d(const std::vector<std::complex<float>>& data);

};

#endif //ETHERTIA_LOADER_H
