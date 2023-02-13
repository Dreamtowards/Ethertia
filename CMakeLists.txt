cmake_minimum_required(VERSION 3.14)
project(Ethertia)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS OFF)
option(CMAKE_BUILD_TYPE Release)
set(BUILD_STATIC OFF)

# there 3 build mode
# 1. Dev: (DEBUG) + (Pre Compiled Shared Library, for faster compile, release-shared-library for faster speed & fastnoise correctness)
# 2. Rel: (RELEASE) + No Shared Library. packed into one. be simple

#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")




set(PROD_BINARY_TARGET "nil")
if (APPLE)
    set(PROD_BINARY_TARGET "darwin-x64")   # x86-64
elseif (WIN32)
    set(PROD_BINARY_TARGET "windows-x64")  # x86-64
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



#add_subdirectory(lib/yaml-cpp)
#link_libraries(yaml-cpp)



if(BUILD_SHARED_LIBS OR BUILD_STATIC)

    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    add_subdirectory(lib/glfw)
    link_libraries(glfw)

    add_subdirectory(lib/bullet3)
    link_libraries(BulletDynamics BulletCollision LinearMath)

    add_subdirectory(lib/fastnoise2)
    link_libraries(FastNoise2)

    add_subdirectory(lib/openal-soft)
    link_libraries(OpenAL)

    add_subdirectory(lib/entt)

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


#set(NBT_BUILD_SHARED ON)  # required for build shared-library.
set(NBT_BUILD_TESTS OFF)
add_subdirectory(lib/nbt)
link_libraries(nbt++)


#add_subdirectory(lib/openvr)
#link_libraries(openvr_api)


include_directories(src)
include_directories(lib)
include_directories(lib/glfw/include)
include_directories(lib/bullet3/src)
include_directories(lib/json/include)
include_directories(lib/enet/include)
include_directories(lib/msgpack/include)
include_directories(lib/fastnoise2/include)
include_directories(lib/entt/src)
include_directories(lib/openal-soft/include)
include_directories(lib/yaml-cpp/include)
include_directories(lib/nbt/include)
include_directories(lib/openvr/headers)



# VK_ICD_FILENAMES = /Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/icd.d/MoltenVK_icd.json;VK_LAYER_PATH = /Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/explicit_layer.d
# VK_ICD_FILENAMES = vulkansdk/macOS/share/vulkan/icd.d/MoltenVK_icd.json
# VK_LAYER_PATH = vulkansdk/macOS/share/vulkan/explicit_layer.d
# https://vulkan.lunarg.com/sdk/home
# include Vulkan SDK.
#include_directories(/Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/include)
#link_directories(/Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/lib)
#link_libraries(  vulkan.1.3.236 )

#if (APPLE) ###    link_libraries(MoltenVK)  # Dont Link MoltenVK Manually. Not anymore.!

add_executable(Ethertia
        lib/glad/glad.c
        lib/tinyfd/tinyfiledialogs.c

        src/ethertia/util/Loader.cpp
        src/ethertia/gui/Gui.cpp
        src/ethertia/render/RenderEngine.cpp
        src/ethertia/render/Window.cpp
        src/ethertia/world/World.cpp
        src/ethertia/Ethertia.cpp
        )

add_executable(DedicatedServer
        src/ethertia/network/DedicatedServer.cpp
        )



add_library(TwilightForest SHARED
        run/mods/TwilightForest/src/TwilightForest.cpp
        )