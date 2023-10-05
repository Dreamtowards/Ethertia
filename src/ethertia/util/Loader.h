//
// Created by Dreamtowards on 2022/3/30.
//

#pragma once

#include <span>
#include <memory>  // std::unique_ptr

#include <vkx/vkx.hpp>

#include <ethertia/render/VertexData.h>
#include <ethertia/util/BitmapImage.h>


class Loader
{
public:

#pragma region File & Asset

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
    static DataBlock LoadFile(const std::string& filename);

    // locate a real filename of an assets-path.
    // return empty string if cannot locate the assets file.
    // if path str leading with './', '/', 'X:/', then just locate actual-path instead of asset-path
    static std::string FindAsset(const std::string& p);

    // a quick utility func of `LoadFile(LocateAsset(p));`
    static DataBlock LoadAsset(const std::string& uri) { return Loader::LoadFile(Loader::FindAsset(uri)); }


    static bool FileExists(const std::string& filename);

    // mkdirs for the "dir/" (slash required) or a file's parent dir "dir/somefile"
    static const std::string& Mkdirs(const std::string& path);

    // !Heavy IO Cost. recursive calc all file size.
    static size_t FileSize(const std::string& path);

#pragma endregion


#pragma region PNG, OBJ


    ///////////////// PNG /////////////////

    // todo: ? return BitmapImage* ptr. for more flexible

    // stbi_load(filename). load file directly.
    static BitmapImage LoadPNG_(const char* filename);

    static BitmapImage LoadPNG(const std::string& uri) { return Loader::LoadPNG_(Loader::FindAsset(uri).c_str()); }


    // stbi_load_from_memory().
    static BitmapImage LoadPNG(const void* data, size_t len);

    // helper func of load_from_memory. allows simply loadPNG(loadFile());
    static BitmapImage LoadPNG(const DataBlock& m) { return LoadPNG(m.data(), m.size()); }



    static void SavePNG(const std::string& filename, const BitmapImage& img);



    ///////////////// OBJ /////////////////


    // internal absolute filename. load indexed unique vertices. backend: tiny_obj_loader.
    static VertexData* LoadOBJ_(const char* filename);

    static VertexData* LoadOBJ(const std::string& uri) { return Loader::LoadOBJ_(Loader::FindAsset(uri).c_str()); }

    // quick save for debug, no compression.
    static void SaveOBJ(const std::string& filename, size_t verts, const float* pos, const float* uv =nullptr, const float* norm =nullptr);



#pragma endregion


#pragma region OGG, WAV
    /*

    ////////////////// SOUNDS: OGG, WAV //////////////////


    // return: PCM data, 16 bit.
    // @out_len: num of samples of pcm data. len*sizeof(int16_t)==size_bytes.
    // @out_channels: 1 or 2. (mono or stereo)
    static int16_t* loadOGG(const DataBlock& data, size_t* out_len, int* out_channels, int* dst_sampleRate);

    // load to OpenAL buffer.
    static AudioBuffer* loadOGG(const DataBlock& data);


    // PCM, 16-bit sample, 1-channel
    static void saveWAV(std::ostream& out, const void* pcm, size_t size, int samplePerSec = 44100);

    */

#pragma endregion


#pragma region Vulkan: VertexBuffer, Image

    // todo: LoadVertexBuffer()  name result, not input. LoadOBJ not LoadOBJFile
    
    // interleaved vertex data. load to GPU, StagedBuffer.
    static vkx::VertexBuffer* LoadVertexData(const VertexData* vtx);

    static vkx::VertexBuffer* LoadVertexData(const std::string& uri) { return Loader::LoadVertexData(std::unique_ptr<VertexData>(Loader::LoadOBJ(uri)).get()); }


    // Image ? Texture
    static vkx::Image* LoadImage(const BitmapImage& img);

    static vkx::Image* LoadImage(const std::string& uri) { return Loader::LoadImage(Loader::LoadPNG(uri)); }


    //////////// CUBE MAP IMAGE ////////////
    /*
    // @imgs 6 Images, order: +X Right, -X Left, +Y Top, -Y Bottom, +Z Front, -Z Back.
    static vkx::Image* loadCubeMap(const BitmapImage* imgs);

    // in the @filename_pattern, "{}" will be replaced with:
    inline static const char* CUBEMAP_PAT[] = {"right", "left", "top", "bottom", "front", "back"};

    // load 6 individual image.
    // @filename_pattern: e.g. "skybox-{}.png", the "{}" will be replaced with @patterns e.g. right/left/top/bottom/front/back.
    static vkx::Image* loadCubeMap_6(const std::string& filename_pattern, const char** patterns = CUBEMAP_PAT);

    // load a 3x2 grid image, first row: bottom, top, back, second row: left front, right
    static vkx::Image* loadCubeMap_3x2(const std::string& filename);

    */
#pragma endregion


#pragma region FileDialog, OS


    ///////////// FileDialogs /////////////


    static void ShowMessageBox(
            const char* title,              // nullable
            const char* message,            // nullable
            const char* dialogType = "ok",  // "ok" "okcancel" "yesno" "yesnocancel"
            const char* iconType = "info",  // "info" "warning" "error" "question"
            int defaultButton = 1);         // 0 for cancel/no , 1 for ok/yes , 2 for no in yesnocancel

    // returns NULL on cancel
    static const char* ShowInputBox(
            const char* title,
            const char* message,
            const char* defaultInput = "");  // if NULL it's a passwordBox


    // in case of multiple files, the separator is |
    static const char* OpenFileDialog(
            const char* title = nullptr,
            const char* defaultPath = nullptr,
            std::initializer_list<const char*> filterPatterns = {},  // NULL or {"*.jpg","*.png"}
            const char* filterDesc = nullptr,                        // NULL or "image files"
            bool allowMultipleSelects = false);


    // returns NULL on cancel
    static const char* SaveFileDialog(
        const char* title = nullptr,
        const char* defaultPath = nullptr,
        std::initializer_list<const char*> filterPatterns = {},  // NULL or {"*.jpg","*.png"}
        const char* singleFilterDesc = nullptr);                 // NULL or "image files"


    static const char* OpenFolderDialog(const char* title = "", const char* defaultPath = "");


    static glm::vec3 OpenColorPicker(
            const char* title,
            glm::vec3 defaultColor,
            const char* defaultHexRGB = nullptr,  // NULL or "#FF0000"
            const char** out_HexRGB = nullptr);   // result e.g. "#FF0000"



    ///////////// OS /////////////



    // open File, Folder, Website URL
    static void OpenURL(std::string_view url);


    // macOS:   darwin-x64  | darwin-arm64
    // Windows: windows-x64 | windows-arm64
    // Linux:   linux-x64   | linux-arm64
    static std::string os_arch();


    static const char* cpuid();


    // https://stackoverflow.com/questions/1984186/what-is-private-bytes-virtual-bytes-working-set
    // @param pPrivateUsage: bytes of memory the process has allocated/asked for. (usually) exclude memory-mapped files (i.e. shared DLLs). but include pagefile usage. 
    // @param pWorkingSet:   non-paged PrivateBytes plus memory-mapped files 
    static bool ram(uint64_t* pPrivateUsage, uint64_t* pWorkingSetSize = nullptr, uint64_t* pUsedPhys = nullptr, uint64_t* pTotalPhys = nullptr);
    // memory_stat

    /*
    // deprecated: no where used.
    // windows / darwin / linux
    // static const char* sys_name();

    // deprecated: no where used. it's originally used for locating mod's binary program.
    // macOS:   lib<Name>.dylib
    // Windows: lib<Name>.dll
    // static std::string sys_libname(const std::string& name);



    ////////////// Misc //////////////


    static std::vector<std::complex<float>> fft_1d(const std::vector<std::complex<float>>& data);
    */

#pragma endregion


};

