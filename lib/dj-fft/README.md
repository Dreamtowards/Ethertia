# dj_fft: Header-only FFT library

[![Build Status](https://travis-ci.org/jdupuy/dj_fft.svg?branch=master)](https://travis-ci.org/jdupuy/dj_fft)
[![Build status](https://ci.appveyor.com/api/projects/status/nwcgmc1l74h8sudk?svg=true)](https://ci.appveyor.com/project/jdupuy/dj-fft)

### Details
This repository provides a header-only library to compute fourier transforms in 1D, 2D, and 3D. Its goal is to provide a fast and easy-to-use fast fourier transform algorithm. 

### Cloning

Clone the repository and all its submodules using the following command:
```sh
git clone --recursive git@github.com:jdupuy/dj_fft.git
```

If you accidentally omitted the `--recursive` flag when cloning the repository you can retrieve the submodules like so:
```sh
git submodule update --init --recursive
```

### Usage
The 1D, 2D, and 3D FFT routines return an `std::vector<std::complex<T>>`, given another `std::vector<std::complex<T>>` as input, which specifies the data that must be transformed, as well as an enum class `dj::fft_dir`, which specifies in which direction the FFT must be computed (specify `dj::fft_dir::DIR_FWD` for the forward direction and `dj::fft_dir::DIR_BWD` for the backward direction).

Note that the input vector is expected to be of size `N` for 1D FFT, `NxN` for a 2D FFT, and `NxNxN` for a 3D FFT, where `N` *must* be a power of two. Note that the 2D and 3D vectors are expected to be arranged in a flat row-major fashion, i.e., the 2D and 3D elements `(i, j)` and `(i, j, k)` are respectively located at index `i + N * j` and `i + N * (j + N * k)` in memory.

Below is a C++ pseudocode for computing a 2D FFT in forward direction:
```c++
#define DJ_FFT_IMPLEMENTATION // define this in exactly *one* .cpp file
#include "dj_fft.h"

some_function()
{
  int N = size_of_your_input; // input size
  auto myData = std::vector<std::complex<T>>(N * N); // input data

  // prepare data
  for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
      myData[i + N * j] = some_value; // set element (i, j)
    }
  }

  // compute forward 2D FFT
  auto fftData = dj::fft2d(myData, dj::fft_dir::DIR_FWD);

  // print the data
  for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
      printf("{%f, %f} ", fftData[i + N * j].real(), fftData[i + N * j].imag());
    }
    printf("\n");
  }
}
```

To see examples that compile, see the examples/ directory. 

### GPU Acceleration
Additionally, the library provides GPU accelerated 1D, 2D, and 3D FFTs for `std::vector<std::complex<float>>` inputs. GPU acceleration is especially relevant for large 2D and 3D datasets. For instance:
- for an input of size 4096x4096, a regular 2D FFT completes in roughly 18 seconds on an intel i7-8086k, and 0.9 seconds on an NVidia RTX 2080
- for an input of size 512x512x512, a regular 3D FFT completes in roughly 131 seconds on an intel i7-8086k, and 8.2 seconds on an NVidia RTX 2080

The following table provides a more comprehensive set of measurements for 2D FFTs:

| 2D FFT Resolution | 256² | 512² | 1024² | 2048² | 4096² | 8192² |
| --- | --- | --- | --- | --- | --- | --- |
| CPU (i7-8086k) | 0.05s | 0.22s | 0.99s | 4.32s | 18.85s | 81.96s |
| GPU (RTX 2080) | 0.01s | 0.02s | 0.07s | 0.24s | 0.94s | 3.68s |
| GPU speed-up | x5 | x11 | x14 | x18 | x20 | x22 |

The following table provides a more comprehensive set of measurements for 3D FFTs:

| 3D FFT Resolution | 64³ | 128³ | 256³ | 512³ |
| --- | --- | --- | --- | --- |
| CPU (i7-8086k) | 0.19s | 1.72s | 15.70s | 141.18s |
| GPU (RTX 2080) | 0.04s | 0.15s | 1.03s | 8.10s |
| GPU speed-up | x5 | x11 | x15 | x17 |

Below is a C++ pseudocode for computing a 1D FFT in backward direction on the GPU:

```c++
#define DJ_FFT_IMPLEMENTATION // define this in exactly *one* .cpp file
#include "dj_fft.h"

some_function()
{
  int N = size_of_your_input; // input size
  auto myData = std::vector<std::complex<float>>(N); // input data

  // prepare data
  for (int i = 0; i < N; ++i) {
    myData[i] = some_float_value; // set element (i)
  }

  // compute backward 1D FFT
  auto fftData = dj::fft1d_gpu(myData, dj::fft_dir::FFT_BWD);

  // print the data
  for (int i = 0; i < N; ++i) {
    printf("{%f, %f}\n", fftData[i].real(), fftData[i].imag());
  }
}
```
Note that the return values of a GPU FFT may differ slightly from that of a regular FFT, due to the way floating point arithmetic is implemented.

For a complete example that compiles, see the examples/ directory.

### GPU Acceleration (Advanced)
By default, the GPU accelerated routines run on the primary GPU. Users who want to run the FFT on a secondary GPU will have to create an OpenGL context themselves and use the `fftNd_gpu_glready` functions. You can create a custom OpenGL context with a cross-platform windowing library such as GLFW (https://www.glfw.org/), and an OpenGL function loader such as glad (https://glad.dav1d.de/). I'll probably add a sample at some point.

### License
This library is in the public domain. You can do anything you want with them. You have no legal obligation to do anything else, although I appreciate attribution.

It is also licensed under the MIT open source license, if you have lawyers who are unhappy with public domain. The `dj_fft.h` source file includes an explicit dual-license for you to choose from.

