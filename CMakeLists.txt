cmake_minimum_required(VERSION 3.17)
project(Ethertia)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
#find_package(OpenAL REQUIRED)

include_directories(src)
include_directories(lib)
include_directories(lib/glfw/include)
include_directories(lib/bullet3/src)

#file(GLOB_RECURSE EthUnits "src/ethertia/*.cpp")

add_executable(Ethertia
        lib/glad/glad.c
        src/ethertia/event/EventBus.cpp

        src/ethertia/lang/symbol/Scope.cpp
        src/ethertia/lang/lexer/TokenType.cpp
        src/ethertia/lang/symbol/SymbolInternalTypes.cpp

        src/ethertia/client/render/renderer/Renderer.cpp
        src/ethertia/block/Block.cpp
        src/ethertia/client/gui/Gui.cpp
        src/ethertia/client/render/RenderEngine.cpp
        src/ethertia/client/Ethertia.cpp )

if (APPLE)
target_link_libraries(Ethertia PRIVATE
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework OpenAL")
endif()

add_subdirectory(lib/glfw)
target_link_libraries(Ethertia PRIVATE glfw)

add_subdirectory(lib/bullet3)
target_link_libraries(Ethertia PRIVATE BulletCollision BulletDynamics LinearMath)
# [examples, Extras, test, build3] are disabled.