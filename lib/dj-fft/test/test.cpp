

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <complex>
#include <algorithm>
#include <chrono>

#include "dj_fft.h"

/*
 * Returns offset to most significant bit
 * NOTE: only works for positive power of 2s
 * examples:
 * 1b      -> 0d
 * 100b    -> 2d
 * 100000b -> 5d
 */
int findMSB(int x)
{
    int p = 0;

    while (x > 1) {
        x>>= 1;
        ++p;
    }

    return p;
}

/*
 *  Bit reverse an integer given a word of nb bits
 *  NOTE: Only works for 32-bit words max
 *  examples:
 *  10b      -> 01b
 *  101b     -> 101b
 *  1011b    -> 1101b
 *  0111001b -> 1001110b
 */
int bitr(uint32_t x, int nb)
{
    x = ( x               << 16) | ( x               >> 16);
    x = ((x & 0x00FF00FF) <<  8) | ((x & 0xFF00FF00) >>  8);
    x = ((x & 0x0F0F0F0F) <<  4) | ((x & 0xF0F0F0F0) >>  4);
    x = ((x & 0x33333333) <<  2) | ((x & 0xCCCCCCCC) >>  2);
    x = ((x & 0x55555555) <<  1) | ((x & 0xAAAAAAAA) >>  1);

    return ((x >> (32 - nb)) & (0xFFFFFFFF >> (32 - nb)));
}

enum class fft_dir {FFT_DIR_FWD = +1, FFT_DIR_BWD = -1};

/*
 * Computes a Fourier transform, i.e.,
 * xo[k] = 1/sqrt(N) sum(j=0 -> N-1) xi[j] exp(i 2pi j k / N)
 * with O(N log N) complexity using the radix-2 algorithm
 *
 * NOTE: Only works for arrays whose size is a power-of-two
 */
template <typename T>
std::vector<std::complex<T>>
fft_1d(const std::vector<std::complex<T>> &xi, const fft_dir &dir)
{
    int cnt = (int)xi.size();
    int msb = findMSB(cnt);
    std::vector<std::complex<T>> xo(cnt);
    T nrm = T(1) / std::sqrt(cnt);

    // pre-process the input data
    for (int j = 0; j < cnt; ++j)
        xo[j] = nrm * xi[bitr(j, msb)];

    // fft passes
    for (int i = 0; i < msb; ++i) {
        int bm = 1 << i; // butterfly mask
        int bw = 2 << i; // butterfly width
        T ang = T(dir) * M_PI / bm; // precomputation

        // fft butterflies
        for (int j = 0; j < (cnt/2); ++j) {
            int i1 = ((j >> i) << (i + 1)) + j % bm; // left wing
            int i2 = i1 ^ bm;                        // right wing
            T a1 = ang * (i1 ^ bw); // left rotation
            T a2 = ang * (i2 ^ bw); // right rotation
            std::complex<T> tmp = xo[i1];

            xo[i1]+= std::polar(T(1), a1) * xo[i2];
            xo[i2] = tmp + std::polar(T(1), a2) * xo[i2];
        }
    }

    return xo;
}

/*
 * 2D fourier transform -- only works with square input
 */
typedef std::complex<float> vec2;
std::vector<vec2> fft_2d(const std::vector<vec2> &xi, const fft_dir &dir)
{
    int cnt2 = (int)xi.size();   // NxN
    int msb = findMSB(cnt2) / 2; // lg2(N) = lg2(sqrt(NxN))
    int cnt = 1 << msb;          // N = 2^lg2(N)
    std::vector<vec2> xo(cnt2);
    float nrm = 1.f / cnt;

    // pre-process the input data
    for (int j2 = 0; j2 < cnt; ++j2)
    for (int j1 = 0; j1 < cnt; ++j1) {
        int k2 = bitr(j2, msb);
        int k1 = bitr(j1, msb);

        xo[j1 + cnt * j2] = nrm * xi[k1 + cnt * k2];
    }

    // fft passes
    for (int i = 0; i < msb; ++i) {
        int bm = 1 << i; // butterfly mask
        int bw = 2 << i; // butterfly width
        float ang = float(dir) * M_PI / bm; // precomputation

        // fft butterflies
        for (int j2 = 0; j2 < (cnt/2); ++j2)
        for (int j1 = 0; j1 < (cnt/2); ++j1) {
            int i11 = ((j1 >> i) << (i + 1)) + j1 % bm; // xmin wing
            int i12 = ((j2 >> i) << (i + 1)) + j2 % bm; // ymin wing
            int i21 = i11 ^ bm;                         // xmax wing
            int i22 = i12 ^ bm;                         // ymax wing
            float ax1 = ang * (i11 ^ bw); // upper left rotation
            float ax2 = ang * (i12 ^ bw); // upper right rotation
            float ay1 = ang * (i21 ^ bw); // lower left rotation
            float ay2 = ang * (i22 ^ bw); // lower right rotation

            i21 = i11 + cnt * i21;
            i22 = i11 + cnt * i22;

            vec2 tmp1 = xo[i11];
            vec2 tmp2 = xo[i21];

            // FFT-X
            xo[i11]+= std::polar(1.f, ax1) * xo[i12];
            xo[i12] = tmp1 + std::polar(1.f, ax2) * xo[i12];
            xo[i21]+= std::polar(1.f, ax1) * xo[i22];
            xo[i22] = tmp2 + std::polar(1.f, ax2) * xo[i22];
            // FFT-Y
            tmp1 = xo[i11];
            tmp2 = xo[i12];
            xo[i11]+= std::polar(1.f, ay1) * xo[i21];
            xo[i21] = tmp1 + std::polar(1.f, ay2) * xo[i21];
            xo[i12]+= std::polar(1.f, ay1) * xo[i22];
            xo[i22] = tmp2 + std::polar(1.f, ay2) * xo[i22];

        }
        printf("\n");
    }

    return xo;
}

#if 0
// backup of original implementation that operated on
// per-element basis rather than per-butterfly
std::vector<vec2> fft_fwd2(const std::vector<vec2> &xi)
{
    int cnt = (int)xi.size();
    int msb = findMSB(cnt);
    int idx = 0;
    std::vector<vec2> xo[2] = {
        std::vector<vec2>(cnt, 0.0f),
        std::vector<vec2>(cnt, 0.0f)
    };

    // pre-process the input data
    for (int j = 0; j < cnt; ++j)
        xo[idx][j] = xi[bitr(j, msb)];

    // fft
    for (int i = 0; i < msb; ++i) {
        const std::vector<vec2> &dataIn  = xo[idx];
        std::vector<vec2> &dataOut = xo[1 - idx];
        int bm = 1 << i; // butterfly mask
        int bw = 2 << i; // butterfly width

        for (int j = 0; j < cnt; ++j) {
            float ang = -2*M_PI / float(bw) * (j % bw);
            std::pair<int, int> lr = std::minmax(j, j ^ bm);

            dataOut[j] = std::polar(1.f, ang) * dataIn[lr.second]
                       + dataIn[lr.first];
        }

        idx = 1 - idx;
    }

    return xo[idx];
}
#endif

// -----------------------------------------------------------------------------
// reference DFT implementation
std::vector<vec2> dft_fwd(const std::vector<vec2> &xi)
{
    int cnt = (int)xi.size();
    std::vector<vec2> xo(cnt, 0.0f);

    for (int j = 0; j < cnt; ++j) {
        float u = j / (float)cnt;
        float ang = -2.0f * M_PI * u;

        for (int k = 0; k < cnt; ++k)
            xo[j]+= xi[k] * std::polar(1.f, ang * k);
    }

    return xo;
}

std::vector<vec2> dft_bwd(const std::vector<vec2> &xi)
{
    int cnt = (int)xi.size();
    std::vector<vec2> xo(cnt, 0.0f);

    for (int j = 0; j < cnt; ++j) {
        float u = j / (float)cnt;
        float ang = 2.0f * M_PI * u;

        for (int k = 0; k < cnt; ++k)
            xo[j]+= xi[k] * std::polar(1.f / (float)cnt, ang * k);
    }

    return xo;
}

float rng()
{
    return (double)rand() / (double)RAND_MAX;
}

int main(int argc, const char **argv)
{
    int cnt = 16;
    std::vector<vec2> xi;

    srand(1278);
    for (int i = 0; i < cnt; ++i)
        xi.push_back(vec2((2.0f * rng() - 1.0f) * 64.f, 0.0f));
/*
    xi = {0.1288654f, 0.89f, 0.545212f, -0.512f,
          21.054, 1.50, -1.041, 0.57};
*/
    auto s1 = std::chrono::high_resolution_clock::now();
    std::vector<vec2> fwd = dft_fwd(xi);
    auto s2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d1 = s2 - s1;
    printf("=> dft: %f s\n", d1.count());

    std::vector<vec2> bwd = dft_bwd(fwd);

    auto s3 = std::chrono::high_resolution_clock::now();
    std::vector<vec2> fwdfft = fft_1d(xi, fft_dir::FFT_DIR_FWD);
    //fwdfft = fft_1d(fwdfft, fft_dir::FFT_DIR_BWD);
    auto s4 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d2 = s4 - s3;
    printf("=> fft: %f s\n", d2.count());

#if 0 // check dft_fwd(dft_bwd)
   for (int i = 0; i < (int)xi.size(); ++i) {
        printf("{%f %f} vs {%f %f}\n", xi[i].real(), xi[i].imag(), bwd[i].real(), bwd[i].imag());
    }
#endif

#if 0 // check dft_fwd vs fft_fwd
    printf("DFT vs FFT\n");
    for (int i = 0; i < (int)xi.size(); ++i) {
        printf("{%f %f}\n", fwd[i].real() - fwdfft[i].real(), fwd[i].imag() - fwdfft[i].imag());
    }
#endif

#if 0 // check fft_fwd output
    printf("DFT vs FFT\n");
    for (int i = 0; i < (int)xi.size(); ++i) {
        printf("{%f %f}\n", fwdfft[i].real(), fwdfft[i].imag());
    }
#endif

#if 0 // FFT 2D
    xi = {
        0.539036, 0.0593635, 0.4717, 0.256344,
        0.910179, 0.300435, 0.376592, 0.991445,
        0.419588, 0.780667, 0.0302993, 0.246948,
        0.0620232, 0.686178, 0.316724, 0.546404
    };
/*
    xi = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9,10,11,
        12,13,14,15
    };
*/
    std::vector<vec2> xo = dj::fft::eval_2d(xi, dj::fft::e_dir::DIR_FWD);

    int n = 4;
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            printf("%f + i%f   ", xo[i+n*j].real(), xo[i+n*j].imag());
        }
        printf("\n");
    }
#endif

#if 1 // FFT 3D
    xi = {
        0, 1, 2, 3,
        4, 5, 6, 7
    };
    xi = {
        -0.0885617, 0.955652, 0.886429, 0.924431, -0.395304, -0.0665829,
        -0.876723, -0.228711, -0.140323, 0.557488, -0.902819, 0.256534,
        -0.444026, -0.819565, 0.753173, -0.781786, -0.468485, 0.837219,
        -0.660168, -0.800843, -0.0596039, -0.193521, 0.943171, -0.370142,
        -0.74844, -0.45548, 0.211495, 0.343527, -0.515198, -0.0163946, 0.125886,
         0.0983918, -0.036781, 0.288511, -0.164898, 0.409043, 0.922663,
        0.807615, 0.740989, 0.263322, -0.215215, -0.127425, 0.136959,
        -0.0341827, -0.67907, 0.926666, -0.907707, -0.00747479, -0.00815844,
        -0.975298, -0.0227348, -0.842961, -0.0856783, 0.265912, 0.0350528,
        0.334472, 0.289027, -0.173865, 0.989117, 0.577764, -0.583729, -0.468592,
        0.0303303, -0.275404
    };

    std::vector<vec2> xo = dj::fft::eval_3d(xi, dj::fft::e_dir::DIR_FWD);

    int n = 4;
    for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                printf("%f + i%f   ", xo[i+n*(j+n*k)].real(), xo[i+n*(j+n*k)].imag());
           }
            printf("\n");
        }
        printf("\n\n");
    }
#endif


    return 0;
}

