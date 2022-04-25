cmake_minimum_required(VERSION 3.17)
project(Ethertia)

set(CMAKE_CXX_STANDARD 14)


include_directories(src)
include_directories(include)


add_executable(Ethertia
        lib/glad.c
        src/ethertia/world/World.cpp
        src/ethertia/client/Window.cpp
        src/ethertia/client/render/renderer/ChunkRenderer.cpp
        src/ethertia/client/render/chunk/ChunkMeshGen.cpp
        src/ethertia/client/Ethertia.cpp)

target_link_libraries(Ethertia PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)

