cmake_minimum_required(VERSION 3.8)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


project(Ethertia)

set(CMAKE_CXX_STANDARD 20)

# perfer use dynamic/shared library for better compile speed and binary size.
set(BUILD_SHARED_LIBS ON)
set(BUILD_STATIC OFF)
#set(CMAKE_BUILD_TYPE "Release")


# Setup MultiThreaded Compilation
#set(CMAKE_BUILD_PARALLEL_LEVEL 10)
#set(MSVC_COMPILE_FLAGS "/MP")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}" )
#set(MAKE_PROGRAM ninja -j10)

#set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(ET_LINUX TRUE)
endif ()



# Setup Build Dir

set(ET_BUILD_TARGET "nil")
if (WIN32)
    set(ET_BUILD_TARGET "windows-x64")       # x86-64
elseif (APPLE)
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(ET_BUILD_TARGET "darwin-arm64")  # apple silicon, arm64
    else()
        set(ET_BUILD_TARGET "darwin-x64")    # x86-64
    endif ()
elseif(ET_LINUX)
    set(ET_BUILD_TARGET "linux-x64")         # x86-64
endif ()
set(ET_BUILD_TARGET_DIR ${CMAKE_SOURCE_DIR}/run/bin/${ET_BUILD_TARGET})

# Output of Shared library & Program
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ET_BUILD_TARGET_DIR}) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ET_BUILD_TARGET_DIR})  # for Windows

message("Ethertia Build Env: ${CMAKE_BUILD_TYPE}; ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}; Bin Dir: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")





#if(WIN32)
#    link_libraries(winmm gdi32 wsock32 ws2_32)  # for enet lib
#
#    if (NOT MSVC)
#        set(CMAKE_CXX_FLAGS "-Wno-deprecated-enum-enum-conversion")  # disable warning for ENET
#    endif ()
#endif()



include_directories(src)
include_directories(lib)

include_directories(lib/glfw-3.3.8/include)
include_directories(lib/imgui-docking)
include_directories(lib/imguizmo-1.83)
include_directories(lib/imgui-imnodes-0.5)
include_directories(lib/imgui-knobs-0.21.2)

include_directories(lib/physx-5.3.0/include)
include_directories(lib/fastnoise2-0.10.0a/include)
include_directories(lib/glm-0.9.9.8)
include_directories(lib/entt-3.12.2/src)

include_directories(lib/enet-2.3.6/include)
include_directories(lib/msgpack-cppack19/include)

include_directories(lib/stb)
include_directories(lib/json-nlohmann-3.11.2/include)
include_directories(lib/tinyobjloader-1.0.6)
include_directories(lib/tinyfd-2.9.3)

include_directories(lib/sol2-3.3.0/include)
include_directories(lib/lua-5.4.6/src)

#include_directories(lib/openxr-1.0.30/include)
#include_directories(lib/openal-soft/include)
#include_directories(lib/openvr-1.26.7/headers)


set(LUA_SRC_DIR "lib/lua-5.4.6/src")
set(LUA_SRC_FILES 
    "${LUA_SRC_DIR}/lapi.c"
    "${LUA_SRC_DIR}/lapi.c"
    "${LUA_SRC_DIR}/lcode.c"
    "${LUA_SRC_DIR}/lctype.c"
    "${LUA_SRC_DIR}/ldebug.c"
    "${LUA_SRC_DIR}/ldo.c"
    "${LUA_SRC_DIR}/ldump.c"
    "${LUA_SRC_DIR}/lfunc.c"
    "${LUA_SRC_DIR}/lgc.c"
    "${LUA_SRC_DIR}/llex.c"
    "${LUA_SRC_DIR}/lmem.c"
    "${LUA_SRC_DIR}/lobject.c"
    "${LUA_SRC_DIR}/lopcodes.c"
    "${LUA_SRC_DIR}/lparser.c"
    "${LUA_SRC_DIR}/lstate.c"
    "${LUA_SRC_DIR}/lstring.c"
    "${LUA_SRC_DIR}/ltable.c"
    "${LUA_SRC_DIR}/ltm.c"
    "${LUA_SRC_DIR}/lundump.c"
    "${LUA_SRC_DIR}/lvm.c"
    "${LUA_SRC_DIR}/lzio.c"
    "${LUA_SRC_DIR}/lauxlib.c"
    "${LUA_SRC_DIR}/lbaselib.c"
    "${LUA_SRC_DIR}/lcorolib.c"
    "${LUA_SRC_DIR}/ldblib.c"
    "${LUA_SRC_DIR}/liolib.c"
    "${LUA_SRC_DIR}/lmathlib.c"
    "${LUA_SRC_DIR}/loadlib.c"
    "${LUA_SRC_DIR}/loslib.c"
    "${LUA_SRC_DIR}/lstrlib.c"
    "${LUA_SRC_DIR}/ltablib.c"
    "${LUA_SRC_DIR}/lutf8lib.c"
    "${LUA_SRC_DIR}/linit.c")


add_executable(Ethertia
    lib/tinyfd-2.9.3/tinyfiledialogs.c
    ${LUA_SRC_FILES}

    #src/ethertia/vr/OpenXR.cpp

    src/vkx/vkx.cpp
    src/stdx/thread_pool.cpp
    src/stdx/str.cpp

    lib/imgui-docking/imgui.cpp
    lib/imgui-docking/imgui_draw.cpp
    lib/imgui-docking/imgui_widgets.cpp
    lib/imgui-docking/imgui_tables.cpp
    lib/imgui-docking/imgui_demo.cpp
    lib/imgui-docking/backends/imgui_impl_glfw.cpp
    lib/imgui-docking/backends/imgui_impl_vulkan.cpp

    lib/imguizmo-1.83/ImGuizmo.cpp
    lib/imgui-knobs-0.21.2/imgui-knobs.cpp
    lib/imgui-imnodes-0.5/imnodes.cpp

    src/ethertia/imgui/Imgui.cpp
    src/ethertia/imgui/Imw.cpp
    src/ethertia/imgui/ImwSettings.cpp
    src/ethertia/imgui/ImwGameplay.cpp
    src/ethertia/imgui/ImwInspector.cpp
    src/ethertia/imgui/ImwProfiler.cpp
    src/ethertia/imgui/ImwDebug.cpp
    src/ethertia/imgui/ImwExplorer.cpp

    src/ethertia/init/init_load.cpp
        
    src/ethertia/util/Loader.cpp
    src/ethertia/util/BitmapImage.cpp
    src/ethertia/util/Maths.cpp
    src/ethertia/render/VertexData.cpp

    src/ethertia/render/Window.cpp
    src/ethertia/render/RenderEngine.cpp
    src/ethertia/init/Controls.cpp
    src/ethertia/init/Settings.cpp
    src/ethertia/world/Physics.cpp
    src/ethertia/mod/Lua.cpp

    src/ethertia/world/World.cpp
    src/ethertia/world/Chunk.cpp
    src/ethertia/world/chunk/ChunkSystem.cpp
    src/ethertia/world/chunk/gen/ChunkGenerator.cpp
    src/ethertia/world/chunk/meshgen/MeshGen.cpp

    # src/ethertia/item/ItemComponents.cpp
    #src/ethertia/render/renderer/RendererGbuffer.cpp    

    src/ethertia/Ethertia.cpp)




add_executable(DedicatedServer
        src/ethertia/network/DedicatedServer.cpp)

add_library(TwilightForest SHARED
        run/mods/TwilightForest/src/TwilightForest.cpp)


        
add_executable(EthertiaTests 
    ${LUA_SRC_FILES}
    src/test/test.cpp)


#On linux, We can build shared libraries as default.  #? why
if((NOT (BUILD_SHARED_LIBS OR BUILD_STATIC)) AND ET_LINUX)
    set(BUILD_SHARED_LIBS ON)
endif()


if(BUILD_SHARED_LIBS)   # OR BUILD_STATIC

    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    add_subdirectory(lib/glfw-3.3.8)
    target_link_libraries(Ethertia glfw)

    add_subdirectory(lib/fastnoise2-0.10.0a)
    target_link_libraries(Ethertia FastNoise2)

#    add_subdirectory(lib/openal-soft)
#    target_link_libraries(Ethertia OpenAL)

endif()





if (MSVC)
    target_compile_options(Ethertia PRIVATE
        /wd4267  # msvc ignore C4267: implicit conversion possible loss of data e.g. uint32_t count = (size_t)ls.size();
        /wd4244
        /wd4305  # warning C4305: 'initializing': truncation from 'double' to 'float'
        /wd4018  # warning C4018: '<': signed/unsigned mismatch
    )
endif()


# Setup Vulkan
set(VULKAN_SDK "C:/Softwares/VulkanSDK/1.3.250.0")
target_include_directories(Ethertia PRIVATE "${VULKAN_SDK}/Include/")
target_link_directories(Ethertia PRIVATE "${VULKAN_SDK}/Lib/")
target_link_libraries(Ethertia "vulkan-1")


# Setup PhysX

# option(PHYSX_BUILD_TYPE "The build type of PhysX, i.e., one of {debug, checked, profile, release}" "checked")
target_link_directories(Ethertia PRIVATE "lib/physx-5.3.0/bin/win.x86_64.vc143.md/debug/")
target_link_libraries(Ethertia 
    "PhysX_64"
    "PhysXFoundation_64"
    "PhysXCommon_64"
    "PhysXCooking_64"
    "PhysXPvdSDK_static_64"
    "PhysXExtensions_static_64"
    "PhysXCharacterKinematic_static_64")
    

#target_compile_options(Ethertia PRIVATE /MTd)
#string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#message("CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")










#set(NBT_BUILD_SHARED ON)  # required for build shared-library.
#set(NBT_BUILD_TESTS OFF)
#add_subdirectory(lib/nbt)
#target_link_libraries(Ethertia nbt++)

#target_link_libraries(Ethertia openvr_api)





# Vulkan, for Apple Darwin need 2 env variables: (no quote)
#VK_DRIVER_FILES=/Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/icd.d/MoltenVK_icd.json    # a.k.a. VK_ICD_FILENAMES, installable client drivers.
#VK_LAYER_PATH = /Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/explicit_layer.d           # for validation layer.

#set(VULKAN_SDK "E:/_Dev/VulkanSDK/1.3.243.0")
#find_package(Vulkan REQUIRED)
#target_link_libraries(target ${Vulkan_LIBRARIES})

#if (APPLE)
#    # VK_ICD_FILENAMES=/Users/dreamtowards/VulkanSDK/1.3.236.0/macOS/share/vulkan/icd.d/MoltenVK_icd.json
#    link_directories(lib/vulkan/bin/darwin-x64)
#    link_libraries(vulkan.1.3.236)
#elseif(WIN32)
#    link_libraries("${CMAKE_SOURCE_DIR}/lib/vulkan/bin/windows-x64/vulkan-1.dll")
#endif ()