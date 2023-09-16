
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define DJ_FFT_IMPLEMENTATION
#include "dj_fft.h"

static void usage(const char *appName)
{
    printf("%s -- FFT for raster image data\n\n", appName);
    printf("usage: %s path_to_image_file\n", appName);
    printf("options:\n"
           "    --help\n"
           "        print help\n"
           "\n"
           "    --imageFormat can be either of:\n"
           "        u8  (--u8)  for uint8 image data file (default)\n"
           "        u16 (--u16) for uint16 image data file\n"
           "        hdr (--hdr) for fp32 image data file\n"
           "\n"
           "\n"
           "    --device can be either of:\n"
           "        cpu (--cpu) runs the FFT on the CPU (default)\n"
           "        gpu (--gpu) runs the FFT on the GPU\n"
           "\n"
           "[NOTE: for square power-of-two resolution images only!]\n"
           );
}

bool is_power_of_two(int x)
{
    return (x & (x - 1)) == 0;
}

bool check_image(int x, int y)
{
    return (x == y) && is_power_of_two(x) && is_power_of_two(y);
}

int main(int argc, char **argv)
{
    enum {FORMAT_UINT8, FORMAT_UINT16, FORMAT_HDR};
    enum {DEVICE_CPU, DEVICE_GPU};
    int format = FORMAT_UINT8;
    int device = DEVICE_CPU;
    const char *imgFile = nullptr;
    dj::fft_arg<float> imgData, imgDataFFT, imgDataInvFFT;
    std::vector<float> fftInput, fftFwdBwd, fftNrm, fftArg;
    int size = 0;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp("--hdr", argv[i])) {
            format = FORMAT_HDR;
        } else if (!strcmp("--u16", argv[i])) {
            format = FORMAT_UINT16;
        } else if (!strcmp("--u8", argv[i])) {
            format = FORMAT_UINT8;
        } else if (!strcmp("--cpu", argv[i])) {
            device = DEVICE_CPU;
        } else if (!strcmp("--gpu", argv[i])) {
            device = DEVICE_GPU;
        } else if (!strcmp("--help", argv[i])) {
            usage(argv[0]);
            return 0;
        } else {
            imgFile = argv[i];
        }
    }

    if (!imgFile) {
        usage(argv[0]);
        return 0;
    }

    // build input data
    if (format == FORMAT_HDR) {
        int x, y, comp;
        const float *texels = stbi_loadf(imgFile, &x, &y, &comp, 1);

        if (!check_image(x, y)) {
            printf("invalid image resolution\n");
            return 0;
        }

        size = x;
        imgData.resize(x * y);
        for (int j = 0; j < y; ++j)
        for (int i = 0; i < x; ++i)
            imgData[i + x * j] = texels[i + x * j];

        stbi_image_free((unsigned char *)texels);
    } else if (format == FORMAT_UINT16) {
        int x, y, comp;
        const uint16_t *texels = stbi_load_16(imgFile, &x, &y, &comp, 1);

        if (!check_image(x, y)) {
            printf("invalid image resolution\n");
            return 0;
        }

        size = x;
        imgData.resize(x * y);
        for (int j = 0; j < y; ++j)
        for (int i = 0; i < x; ++i)
            imgData[i + x * j] = float(texels[i + x * j]) / 64535.f;

        stbi_image_free((unsigned char *)texels);
    } else {
        int x, y, comp;
        const uint8_t *texels = stbi_load(imgFile, &x, &y, &comp, 1);

        if (!check_image(x, y)) {
            printf("invalid image resolution\n");
            return 0;
        }

        size = x;
        imgData.resize(x * y);
        for (int j = 0; j < y; ++j)
        for (int i = 0; i < x; ++i)
            imgData[i + x * j] = float(texels[i + x * j]) / 255.f;

        stbi_image_free((unsigned char *)texels);
    }

    // compute FFT and inverse FFT on the CPU or the GPU
    if (device == DEVICE_CPU) {
        imgDataFFT = dj::fft2d(imgData, dj::fft_dir::DIR_FWD);
        imgDataInvFFT = dj::fft2d(imgDataFFT, dj::fft_dir::DIR_BWD);
    } else if (device == DEVICE_GPU) {
        imgDataFFT = dj::fft2d_gpu(imgData, dj::fft_dir::DIR_FWD);
        imgDataInvFFT = dj::fft2d_gpu(imgDataFFT, dj::fft_dir::DIR_BWD);
    }

    // build norm array and phase array
    fftNrm.resize(imgDataFFT.size());
    fftArg.resize(imgDataFFT.size());
    fftFwdBwd.resize(imgDataFFT.size());
    fftInput.resize(imgDataFFT.size());

    // create spectrum images
    for (int i = 0; i < (int)imgDataFFT.size(); ++i) {
        // the spectrum data is shifted by size/2
        // so that the 0 frequency is located at the center of the image
        int i1 = (i / size + size / 2) % size;
        int i2 = (i % size + size / 2) % size;
        int j = i1 + size * i2;
        fftNrm[j] = std::abs(imgDataFFT[i]);
        fftArg[j] = std::arg(imgDataFFT[i]);
        if (fftArg[j] < 0.0f) fftArg[j]+= /*Pi*/std::acos(-1.f);

        // also export input and output for validation
        fftInput[i] = imgData[i].real();
        fftFwdBwd[i] = imgDataInvFFT[i].real();
    }

    // write to .hdr file
    stbi_write_hdr("fft_input.hdr", size, size, 1, &fftInput[0]);
    stbi_write_hdr("fft_fwd_bwd.hdr", size, size, 1, &fftFwdBwd[0]);
    stbi_write_hdr("fft_nrm.hdr", size, size, 1, &fftNrm[0]);
    stbi_write_hdr("fft_arg.hdr", size, size, 1, &fftArg[0]);

    return 0;
}
