cmake_minimum_required(VERSION 3.17)
project(Ethertia)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
#find_package(OpenAL REQUIRED)
#file(GLOB_RECURSE Eths "src/ethertia/*.cpp")

include_directories(src)
include_directories(lib)
include_directories(lib/glfw/include)
include_directories(lib/bullet3/src)
include_directories(lib/fmt/include)

add_executable(Ethertia
        lib/glad/glad.c
        src/ethertia/event/EventBus.cpp

        src/ethertia/lang/symbol/Scope.cpp
        src/ethertia/lang/lexer/TokenType.cpp
        src/ethertia/lang/symbol/SymbolInternalTypes.cpp

        src/ethertia/util/launcher/Launcher.cpp
#        src/ethertia/skycolor.cpp
#        src/ethertia/gui/Gui.cpp
#        src/ethertia/render/RenderEngine.cpp
#        src/ethertia/Ethertia.cpp
        )

if (APPLE)
target_link_libraries(Ethertia PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework OpenAL")
endif()

#if (WIN32)
##MinGW, 'x86_64-12.2.0-release-posix-sjlj-rt_v10-rev0.7z', add mingw64/bin to PATH without space.
#target_link_libraries(Ethertia PRIVATE
#        "-static-libgcc"
#        "-static-libstdc++")
#endif()



# lib glfw
add_subdirectory(lib/glfw)
target_link_libraries(Ethertia PRIVATE glfw)



# lib bullet3
add_subdirectory(lib/bullet3)
target_link_libraries(Ethertia PRIVATE BulletDynamics BulletCollision LinearMath)
# [examples, Extras, test, build3] are disabled.



# lib fmt
add_subdirectory(lib/fmt)
target_link_libraries(Ethertia PRIVATE fmt::fmt)


# lib libui
add_subdirectory(lib/libui)
target_link_libraries(Ethertia PRIVATE ui)