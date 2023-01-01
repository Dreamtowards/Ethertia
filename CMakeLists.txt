cmake_minimum_required(VERSION 3.14)
project(Ethertia)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_SHARED_LIBS OFF)
option(CMAKE_BUILD_TYPE Release)

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


if(BUILD_SHARED_LIBS)

    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    add_subdirectory(lib/glfw)

    add_subdirectory(lib/bullet3)
    add_subdirectory(lib/fastnoise)
    add_subdirectory(lib/entt)

    link_libraries(glfw BulletDynamics BulletCollision LinearMath FastNoise2)

else()
    link_directories(${PROD_BINARY_DIR})

    if (APPLE)
        link_libraries(BulletDynamics.3.25 BulletCollision.3.25 LinearMath.3.25
                       glfw.3 FastNoise)
    endif ()
    if(WIN32)
        link_libraries(BulletDynamics BulletCollision LinearMath
                       glfw3 FastNoise)

        link_libraries(winmm gdi32 wsock32 ws2_32)  # for enet lib
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
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-enum-enum-conversion")  # disable warning for ENET
endif ()

