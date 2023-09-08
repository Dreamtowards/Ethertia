cmake_minimum_required(VERSION 3.14)
project(Ethertia)

set(CMAKE_CXX_STANDARD 20)

# we manually build shared-lib only the first time. (if doesn't have compiled libs), then we just use/link them instead of recompile them.
set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC OFF)
set(CMAKE_BUILD_TYPE Debug)

#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
endif ()

# BUILD_TARGET = {windows-x64|darwin-arm64|darwin-x64|linux-x64}
set(BUILD_TARGET "nil")
if (WIN32)
    set(BUILD_TARGET "windows-x64")       # x86-64
elseif (APPLE)
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(BUILD_TARGET "darwin-arm64")  # apple silicon, arm64
    else()
        set(BUILD_TARGET "darwin-x64")    # x86-64
    endif ()
elseif(LINUX)
    set(BUILD_TARGET "linux-x64")         # x86-64
endif ()
set(BUILD_TARGET_DIR ${CMAKE_SOURCE_DIR}/run/bin/${BUILD_TARGET})

# Output of Shared library & Program
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_TARGET_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_TARGET_DIR})  # for Windows

message("Build Env: ${CMAKE_SYSTEM_PROCESSOR}; ${CMAKE_SYSTEM_NAME}")
message("Bin Dir: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")



if(WIN32)
    link_libraries(winmm gdi32 wsock32 ws2_32)  # for enet lib

    if (NOT MSVC)
        set(CMAKE_CXX_FLAGS "-Wno-deprecated-enum-enum-conversion")  # disable warning for ENET
    endif ()
endif()


#if (APPLE)
#    # VK_ICD_FILENAMES=/Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/icd.d/MoltenVK_icd.json
#    link_directories(lib/vulkan/bin/darwin-x64)
#    link_libraries(vulkan.1.3.236)
#elseif(WIN32)
#    link_libraries("${CMAKE_SOURCE_DIR}/lib/vulkan/bin/windows-x64/vulkan-1.dll")
#endif ()


include_directories(src)
include_directories(lib)
include_directories(lib/glfw/include)
include_directories(lib/bullet3/src)
include_directories(lib/json/include)
include_directories(lib/enet/include)
include_directories(lib/msgpack/include)
include_directories(lib/fastnoise2/include)
#include_directories(lib/glad45/include)
include_directories(lib/entt/src)
include_directories(lib/openal-soft/include)
include_directories(lib/yaml-cpp/include)
include_directories(lib/nbt/include)
include_directories(lib/openvr/headers)
include_directories(lib/imgui-docking)
include_directories(lib/physx/include)

#include_directories(lib/vulkan/include)


add_executable(Ethertia
#        lib/glad45/src/glad.c
        lib/tinyfd/tinyfiledialogs.c

        lib/imgui-docking/imgui.cpp
        lib/imgui-docking/imgui_draw.cpp
        lib/imgui-docking/imgui_widgets.cpp
        lib/imgui-docking/imgui_tables.cpp
        lib/imgui-docking/imgui_demo.cpp
        lib/imgui-docking/backends/imgui_impl_glfw.cpp
        lib/imgui-docking/backends/imgui_impl_vulkan.cpp
#        lib/imgui-docking/backends/imgui_impl_opengl3.cpp

        lib/imguizmo/ImGuizmo.cpp
        lib/imgui-knobs/imgui-knobs.cpp
        lib/imgui-imnodes/imnodes.cpp

        src/ethertia/imgui/Imgui.cpp
        src/ethertia/util/Log.cpp
        src/ethertia/util/Loader.cpp
        src/ethertia/util/BitmapImage.cpp
        src/ethertia/util/BenchmarkTimer.cpp
        src/ethertia/util/Strings.cpp

        src/ethertia/init/Controls.cpp
        src/ethertia/init/Settings.cpp
        src/ethertia/render/Window.cpp
        src/ethertia/render/RenderEngine.cpp
#        src/ethertia/render/ShaderProgram.cpp
#        src/ethertia/render/RenderCommand_ImplGL.cpp
        src/ethertia/render/VertexData.cpp
        src/ethertia/item/ItemComponents.cpp
        src/ethertia/world/World.cpp

#        src/ethertia/render/vkx/vkx.cpp
#        src/ethertia/render/glc/glc.cpp

        src/vkx/vkx.cpp

        src/ethertia/Ethertia.cpp
        )

add_executable(Elytra
        src/ethertia/lang/Elytra.cpp
        )


#add_executable(Eldaria
#        lib/imgui-docking/imgui.cpp
#        lib/imgui-docking/imgui_draw.cpp
#        lib/imgui-docking/imgui_widgets.cpp
#        lib/imgui-docking/imgui_tables.cpp
#        lib/imgui-docking/imgui_demo.cpp
#        lib/imgui-docking/backends/imgui_impl_glfw.cpp
#        lib/imgui-docking/backends/imgui_impl_vulkan.cpp
#        lib/tinyfd/tinyfiledialogs.c
#
#        src/eldaria/Eldaria.cpp
#        src/eldaria/util/Loader.cpp
#        src/ethertia/util/Loader.cpp
#        src/ethertia/render/VertexData.cpp
#        src/eldaria/vulkan/VulkanIntl.cpp
#        src/eldaria/imgui/Imgui.cpp)


add_executable(DedicatedServer
        src/ethertia/network/DedicatedServer.cpp
        )

add_library(TwilightForest SHARED
        run/mods/TwilightForest/src/TwilightForest.cpp
        )


#On linux, We can build shared libraries as default.
if((NOT (BUILD_SHARED_LIBS OR BUILD_STATIC)) AND LINUX)
    set(BUILD_SHARED_LIBS ON)
endif()


if(BUILD_SHARED_LIBS OR BUILD_STATIC)

    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    add_subdirectory(lib/glfw)
    target_link_libraries(Ethertia glfw)

    add_subdirectory(lib/bullet3)
    target_link_libraries(Ethertia BulletDynamics BulletCollision LinearMath)

    add_subdirectory(lib/fastnoise2)
    target_link_libraries(Ethertia FastNoise2)

    add_subdirectory(lib/openal-soft)
    target_link_libraries(Ethertia OpenAL)

    add_subdirectory(lib/entt)

else()
    target_link_directories(Ethertia PUBLIC ${BIN_DIR})
#    target_link_directories(Eldaria PUBLIC ${BIN_DIR})

    target_link_libraries(Ethertia BulletDynamics BulletCollision LinearMath FastNoise)

    if (APPLE)
        target_link_libraries(Ethertia glfw.3 openal)
#        target_link_libraries(Eldaria glfw.3 openal)
    endif ()
    if(WIN32)
        target_link_libraries(Ethertia glfw3 OpenAL32)
#        target_link_libraries(Eldaria glfw3 OpenAL32)
    endif()
endif()





#set(NBT_BUILD_SHARED ON)  # required for build shared-library.
set(NBT_BUILD_TESTS OFF)
add_subdirectory(lib/nbt)
target_link_libraries(Ethertia nbt++)


#target_link_libraries(Ethertia openvr_api)



if (MSVC)
    target_compile_options(Ethertia PRIVATE
        /wd4267  # msvc ignore C4267: implicit conversion possible loss of data e.g. uint32_t count = (size_t)ls.size();
        /wd4244
    )
endif()

target_include_directories(Ethertia PRIVATE
        "C:/Softwares/VulkanSDK/1.3.250.0/Include/")

target_link_directories(Ethertia PRIVATE
        "C:/Softwares/VulkanSDK/1.3.250.0/Lib/")

target_link_libraries(Ethertia
        "vulkan-1")


#add_subdirectory(lib/openvr)

#add_subdirectory(lib/yaml-cpp)
#link_libraries(yaml-cpp)


# https://vulkan.lunarg.com/sdk/home

# Vulkan, for Apple Darwin need 2 env variables: (no quote)
#VK_DRIVER_FILES=/Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/icd.d/MoltenVK_icd.json    # a.k.a. VK_ICD_FILENAMES, installable client drivers.
#VK_LAYER_PATH = /Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/explicit_layer.d           # for validation layer.



#set(VULKAN_SDK "E:/_Dev/VulkanSDK/1.3.243.0")
#find_package(Vulkan REQUIRED)
#target_link_libraries(target ${Vulkan_LIBRARIES})

#if (APPLE) ###    link_libraries(MoltenVK)  # Dont Link MoltenVK Manually. it's linked by ICD loaders.