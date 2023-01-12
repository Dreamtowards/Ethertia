#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <chrono>

#define DJ_FFT_IMPLEMENTATION
#include "dj_fft.h"

#define LOG(fmt, ...)  fprintf(stdout, fmt, ##__VA_ARGS__); fflush(stdout);

float rng()
{
    return (double)rand() / (double)RAND_MAX;
}

int main(int argc, char **argv)
{
#if 1 // perform a 1D FFT
    {
        srand(1234);
        int cnt = 16;
        dj::fft_arg<float> xi;

        for (int i = 0; i < cnt; ++i)
            xi.push_back(std::complex<float>((2.0f * rng() - 1.0f) * 64.f, 0.0f));

        auto s1 = std::chrono::high_resolution_clock::now();
        dj::fft_arg<float> cpu = dj::fft1d(xi, dj::fft_dir::DIR_FWD);
        auto s2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d1 = s2 - s1;
        printf("=> [%i] CPU: %f s\n", cnt, d1.count()); fflush(stdout);

        auto s3 = std::chrono::high_resolution_clock::now();
        dj::fft_arg<float> gpu = dj::fft1d_gpu(xi, dj::fft_dir::DIR_FWD);
        auto s4 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d2 = s4 - s3;
        printf("=> [%i] GPU: %f s\n", cnt, d2.count()); fflush(stdout);

#if 1 // display content
    for (int i = 0; i < cpu.size(); ++i) {
        printf("{%f %f} vs {%f %f}\n",
               cpu[i].real(), cpu[i].imag(),
               gpu[i].real(), gpu[i].imag());
    }
#endif
    }
#endif

#if 1 // perform a 2D FFT
    {
        srand(3234);
        int cnt = 8;
        dj::fft_arg<float> xi;

        for (int i = 0; i < cnt * cnt; ++i)
            xi.push_back(std::complex<float>((2.0f * rng() - 1.0f) * 64.f, 0.0f));

        auto s1 = std::chrono::high_resolution_clock::now();
        dj::fft_arg<float> cpu = dj::fft2d(xi, dj::fft_dir::DIR_FWD);
        auto s2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d1 = s2 - s1;
        printf("=> [%i^2] CPU: %f s\n", cnt, d1.count()); fflush(stdout);

        auto s3 = std::chrono::high_resolution_clock::now();
        dj::fft_arg<float> gpu = dj::fft2d_gpu(xi, dj::fft_dir::DIR_FWD);
        auto s4 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d2 = s4 - s3;
        printf("=> [%i^2] GPU: %f s\n", cnt, d2.count()); fflush(stdout);

#if 1 // display content
        for (int j = 0; j < cnt; ++j) {
            for (int i = 0; i < cnt; ++i) {
                printf("{%f %f} vs {%f %f}\n",
                       cpu[i+cnt*j].real(), cpu[i+cnt*j].imag(),
                       gpu[i+cnt*j].real(), gpu[i+cnt*j].imag());
             }
            printf("\n");
        }
#endif
    }
#endif

#if 1 // perform a 3D FFT
    {
        srand(3234);
        int cnt = 4;
        dj::fft_arg<float> xi;

        for (int i = 0; i < (cnt * cnt * cnt); ++i)
            xi.push_back(std::complex<float>((2.0f * rng() - 1.0f) * 64.f, 0.0f));

        auto s1 = std::chrono::high_resolution_clock::now();
        dj::fft_arg<float> cpu = dj::fft3d(xi, dj::fft_dir::DIR_FWD);
        auto s2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d1 = s2 - s1;
        printf("=> [%i^3] CPU: %f s\n", cnt, d1.count()); fflush(stdout);

        auto s3 = std::chrono::high_resolution_clock::now();
        dj::fft_arg<float> gpu = dj::fft3d_gpu(xi, dj::fft_dir::DIR_FWD);
        auto s4 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d2 = s4 - s3;
        printf("=> [%i^3] GPU: %f s\n", cnt, d2.count()); fflush(stdout);

#if 1 // display content
        for (int k = 0; k < cnt; ++k) {
            for (int i = 0; i < cnt; ++i) {
                for (int j = 0; j < cnt; ++j) {
                    printf("{%f %f} vs {%f %f}\n",
                           cpu[i+cnt*(j+cnt*k)].real(), cpu[i+cnt*(j+cnt*k)].imag(),
                           gpu[i+cnt*(j+cnt*k)].real(), gpu[i+cnt*(j+cnt*k)].imag());
                 }
                printf("\n");
            }
            printf("\n\n");
        }
#endif
    }
#endif

    return 0;
}

