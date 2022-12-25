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
#include_directories(lib/asio/include)
include_directories(lib/enet/include)
include_directories(lib/msgpack/include)




add_executable(Ethertia
        lib/glad/glad.c
        # src/ethertia/event/EventBus.cpp

        src/ethertia/gui/Gui.cpp
        src/ethertia/render/RenderEngine.cpp
        src/ethertia/Ethertia.cpp
        )

# lib glfw
add_subdirectory(lib/glfw)
target_link_libraries(Ethertia PRIVATE glfw)

# lib bullet3
add_subdirectory(lib/bullet3)
target_link_libraries(Ethertia PRIVATE BulletDynamics BulletCollision LinearMath)
# [examples, Extras, test, build3] are disabled.




add_executable(DedicatedServer
        src/ethertia/server/DedicatedServer.cpp
        )


if (APPLE)
    target_link_libraries(Ethertia PRIVATE
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework IOKit"
            "-framework OpenAL")
endif()
if (WIN32)
    #   mingw-w64 WinLibs build, add mingw64/bin to PATH without space. "winlibs-x86_64-posix-seh-gcc-12.2.0-llvm-14.0.6-mingw-w64ucrt-10.0.0-r2.zip" 'https://winlibs.com/'

    # have order.
    target_link_libraries(Ethertia        PRIVATE winmm gdi32 wsock32 ws2_32)
    target_link_libraries(DedicatedServer PRIVATE winmm gdi32 wsock32 ws2_32)
endif()





add_executable(Elytra
        src/ethertia/lang/symbol/Scope.cpp
        src/ethertia/lang/lexer/TokenType.cpp
        src/ethertia/lang/symbol/SymbolInternalTypes.cpp
        )



## lib fmt
#add_subdirectory(lib/fmt)
#target_link_libraries(Ethertia PRIVATE fmt::fmt)



#target_precompile_headers(Ethertia PRIVATE
#        <algorithm>
#        "lib/json/include/nlohmann/json.hpp"
#        )

#set(CMAKE_C_COMPILER ccache gcc)
#set(CMAKE_CXX_COMPILER ccache g++)

#find_program(CCACHE_PROGRAM ccache)
#if(CCACHE_PROGRAM)
#    message(STATUS "Set up ccache ...")
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
#endif()






#add_executable(Launcher
#        lib/tinyfd/tinyfiledialogs.c
#        src/ethertia/util/launcher/Launcher.cpp
#        )
#
## lib libui
#add_subdirectory(lib/libui)
#target_link_libraries(Launcher PRIVATE libui)



