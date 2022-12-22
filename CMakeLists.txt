cmake_minimum_required(VERSION 3.14)
project(Ethertia)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
#find_package(OpenAL REQUIRED)

include_directories(src)
include_directories(lib)
include_directories(lib/glfw/include)
include_directories(lib/bullet3/src)
include_directories(lib/json/include)
#include_directories(lib/fmt/include)



add_executable(Ethertia
        lib/glad/glad.c
        src/ethertia/event/EventBus.cpp

        src/ethertia/gui/Gui.cpp
        src/ethertia/render/RenderEngine.cpp
        src/ethertia/Ethertia.cpp
        )

if (APPLE)
target_link_libraries(Ethertia PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework OpenAL")
endif()

# if (WIN32) mingw-w64 WinLibs build, add mingw64/bin to PATH without space.
# "winlibs-x86_64-posix-seh-gcc-12.2.0-llvm-14.0.6-mingw-w64ucrt-10.0.0-r2.zip" 'https://winlibs.com/'


# lib glfw
add_subdirectory(lib/glfw)
target_link_libraries(Ethertia PRIVATE glfw)


# lib bullet3
add_subdirectory(lib/bullet3)
target_link_libraries(Ethertia PRIVATE BulletDynamics BulletCollision LinearMath)
# [examples, Extras, test, build3] are disabled.


## lib fmt
#add_subdirectory(lib/fmt)
#target_link_libraries(Ethertia PRIVATE fmt::fmt)




add_executable(EthertiaServer
        src/ethertia/server/DedicatedServer.cpp
        )

add_executable(TesCli
        src/ethertia/server/TestCli.cpp
        )




#add_executable(Launcher
#        lib/tinyfd/tinyfiledialogs.c
#        src/ethertia/util/launcher/Launcher.cpp
#        )
#
## lib libui
#add_subdirectory(lib/libui)
#target_link_libraries(Launcher PRIVATE libui)





add_executable(Elytra
        src/ethertia/lang/symbol/Scope.cpp
        src/ethertia/lang/lexer/TokenType.cpp
        src/ethertia/lang/symbol/SymbolInternalTypes.cpp
        )