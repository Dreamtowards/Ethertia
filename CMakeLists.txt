cmake_minimum_required(VERSION 3.14)
project(Ethertia)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS OFF)
option(CMAKE_BUILD_TYPE Release)
set(BUILD_DISTRO OFF)

# there 3 build mode
# 1. Dev: (DEBUG) + (Pre Compiled Shared Library, for faster compile, release-shared-library for faster speed & fastnoise correctness)
# 2. Rel: (RELEASE) + No Shared Library. packed into one. be simple

#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


set(PROD_BINARY_TARGET "nil")
if (APPLE)
    set(PROD_BINARY_TARGET "macos-x86-64")
elseif (WIN32)
    set(PROD_BINARY_TARGET "windows-amd64")
endif ()
set(PROD_BINARY_DIR ${CMAKE_SOURCE_DIR}/run/bin/${PROD_BINARY_TARGET})

# Output of Shared library & Program
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROD_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROD_BINARY_DIR})  # for Windows

message("Build Env: ${CMAKE_SYSTEM_PROCESSOR}; ${CMAKE_SYSTEM_NAME}")
message("Binary Dir: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

if(WIN32)
    link_libraries(winmm gdi32 wsock32 ws2_32)  # for enet lib

    set(CMAKE_CXX_FLAGS "-Wno-deprecated-enum-enum-conversion")  # disable warning for ENET
endif()

if(BUILD_SHARED_LIBS OR BUILD_DISTRO)

    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    add_subdirectory(lib/glfw)

    add_subdirectory(lib/bullet3)
    add_subdirectory(lib/fastnoise)
    add_subdirectory(lib/entt)
    add_subdirectory(lib/openal-soft)

    link_libraries(glfw BulletDynamics BulletCollision LinearMath FastNoise2 OpenAL)

else()
    link_directories(${PROD_BINARY_DIR})

    link_libraries(BulletDynamics BulletCollision LinearMath
                   FastNoise)

    if (APPLE)
        link_libraries(glfw.3 openal)
    endif ()
    if(WIN32)
        link_libraries(glfw3 OpenAL32)
    endif()
endif()




include_directories(src)
include_directories(lib)
include_directories(lib/glfw/include)
include_directories(lib/bullet3/src)
include_directories(lib/json/include)
include_directories(lib/enet/include)
include_directories(lib/msgpack/include)
include_directories(lib/fastnoise/include)
include_directories(lib/entt/src)
include_directories(lib/openal-soft/include)


add_executable(Ethertia
        lib/glad/glad.c

        src/ethertia/gui/Gui.cpp
        src/ethertia/render/RenderEngine.cpp
        src/ethertia/world/World.cpp
        src/ethertia/Ethertia.cpp
        )

add_executable(DedicatedServer
        src/ethertia/network/DedicatedServer.cpp)




if (WIN32)
endif ()

