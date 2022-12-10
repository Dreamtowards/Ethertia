cmake_minimum_required(VERSION 3.4)
project(ui)

option(LIBUI_BUILD_EXAMPLES "Build libui examples" ON)
option(LIBUI_BUILD_SHARED_LIBS "Build libui as a shared library" ${BUILD_SHARED_LIBS})

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "_d")
endif()

set(DIR_LIBUI ".")

file(GLOB_RECURSE COMMON_SOURCES ${DIR_LIBUI}/common/*.c)
file(GLOB_RECURSE COMMON_HEADERS ${DIR_LIBUI}/common/*.h)
file(GLOB_RECURSE UNIX_SOURCES ${DIR_LIBUI}/unix/*.c)
file(GLOB_RECURSE UNIX_HEADERS ${DIR_LIBUI}/unix/*.h)
file(GLOB_RECURSE WINDOWS_SOURCES ${DIR_LIBUI}/windows/*.cpp)
file(GLOB_RECURSE WINDOWS_HEADERS ${DIR_LIBUI}/windows/*.hpp)
file(GLOB_RECURSE DARWIN_SOURCES ${DIR_LIBUI}/darwin/*.m)
file(GLOB_RECURSE DARWIN_HEADERS ${DIR_LIBUI}/darwin/*.h)

if(WIN32)
    set(SOURCES ${COMMON_SOURCES} ${WINDOWS_SOURCES})
    set(HEADERS ${COMMON_HEADERS} ${WINDOWS_HEADERS})
    set(LIBUI_PLATFORM_HEADER ${DIR_LIBUI}/ui_windows.h)
    set(LIBUI_RC_FILE ${DIR_LIBUI}/windows/resources.rc)
elseif(APPLE)
    set(SOURCES ${COMMON_SOURCES} ${DARWIN_SOURCES})
    set(HEADERS ${COMMON_HEADERS} ${DARWIN_HEADERS})
    set(LIBUI_PLATFORM_HEADER ${DIR_LIBUI}/ui_darwin.h)
else()
    set(SOURCES ${COMMON_SOURCES} ${UNIX_SOURCES})
    set(HEADERS ${COMMON_HEADERS} ${UNIX_HEADERS})
    set(LIBUI_PLATFORM_HEADER ${DIR_LIBUI}/ui_unix.h)
endif()
set(LIBUI_HEADER ${DIR_LIBUI}/ui.h)


if(LIBUI_BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${LIBUI_RC_FILE})
    add_compile_definitions(${PROJECT_NAME} PRIVATE libui_EXPORTS=1)
else()
    add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${LIBUI_HEADER};${LIBUI_PLATFORM_HEADER}")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
if (CMAKE_DEBUG_POSTFIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
endif()

if(WIN32)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
        if(LIBUI_BUILD_SHARED_LIBS)
            #target_link_options(${PROJECT_NAME} PRIVATE /MANIFEST:NO)
            set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO")
        endif()
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
    target_link_libraries(${PROJECT_NAME} D2d1 Dwrite windowscodecs UxTheme Comctl32)
elseif(APPLE)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(COCOA_LIBRARY Cocoa)
    if(LIBUI_BUILD_SHARED_LIBS)
        target_link_libraries(${PROJECT_NAME}
                ${FOUNDATION_LIBRARY}
                ${COCOA_LIBRARY}
                )
    endif()
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_search_module(GLIB REQUIRED glib-2.0)
    target_include_directories(${PROJECT_NAME} PRIVATE
            ${GTK3_INCLUDE_DIRS}
            ${GLIB_INCLUDE_DIRS}
            )
    if(LIBUI_BUILD_SHARED_LIBS)
        target_link_libraries(${PROJECT_NAME}
                ${GTK3_LIBRARIES}
                ${GLIB_LDFLAGS}
                ${CMAKE_DL_LIBS}
                m
                )
    endif()
endif()

if(LIBUI_BUILD_EXAMPLES)
    set(EXAMPLES controlgallery datetime drawtext histogram timer)
    foreach(EXAMPLE ${EXAMPLES})
        file(GLOB EXAMPLE_SOURCES ${DIR_LIBUI}/examples/${EXAMPLE}/main.*)

        if(WIN32)
            set(EXAMPLE_SOURCES ${EXAMPLE_SOURCES} ${LIBUI_RC_FILE})
        endif()

        add_executable(${EXAMPLE} ${EXAMPLE_SOURCES})
        target_include_directories(${EXAMPLE} PRIVATE ${DIR_LIBUI})
        target_link_libraries(${EXAMPLE} ${PROJECT_NAME})
        add_dependencies(${EXAMPLE} ${PROJECT_NAME})
        set_target_properties(${EXAMPLE} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

        if(WIN32)
            if(NOT LIBUI_BUILD_SHARED_LIBS)
                target_link_libraries(${EXAMPLE} D2d1 Dwrite windowscodecs UxTheme Comctl32)
                if(MSVC)
                    #target_link_options(${EXAMPLE} PRIVATE /MANIFEST:NO)
                    set_target_properties(${EXAMPLE} PROPERTIES LINK_FLAGS "/MANIFEST:NO")
                endif()
            endif()
        elseif(APPLE)
            if(NOT LIBUI_BUILD_SHARED_LIBS)
                target_link_libraries(${EXAMPLE}
                        ${FOUNDATION_LIBRARY}
                        ${COCOA_LIBRARY}
                        )
            endif()
        else()
            if(NOT LIBUI_BUILD_SHARED_LIBS)
                target_link_libraries(${EXAMPLE}
                        ${GTK3_LIBRARIES}
                        ${GLIB_LDFLAGS}
                        ${CMAKE_DL_LIBS}
                        m
                        )
            endif()
        endif()
    endforeach()
endif()

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT library
        PUBLIC_HEADER DESTINATION include/libui
        )

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_SOURCE_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "PackageMake")
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_SOURCE_GENERATOR "TGZ")
endif()
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Matus Novak")
include(CPack)