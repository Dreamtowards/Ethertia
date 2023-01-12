cmake_minimum_required (VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)


include_directories(..)
include_directories(submodules)

if (WIN32)
    list (APPEND LIBS OpenGL32)
elseif (UNIX)
    list (APPEND LIBS X11 GL dl)
endif()

project (dj_fft)

add_executable(img_to_fft2d img_to_fft2d.cpp)
target_link_libraries(img_to_fft2d ${LIBS})

add_executable(gpu_acceleration gpu_acceleration.cpp submodules/glad/glad.c)
target_link_libraries(gpu_acceleration ${LIBS})
