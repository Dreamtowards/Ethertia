cmake_minimum_required(VERSION 3.17)
project(Ethertia)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")


include_directories(src)
include_directories(include)

#file(GLOB_RECURSE EthUnits "src/ethertia/*.cpp")

add_executable(Ethertia
        lib/glad.c
        src/ethertia/client/render/renderer/ChunkRenderer.cpp
        src/ethertia/client/Window.cpp
        src/ethertia/world/World.cpp
        src/ethertia/client/Ethertia.cpp)

target_link_libraries(Ethertia PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)

