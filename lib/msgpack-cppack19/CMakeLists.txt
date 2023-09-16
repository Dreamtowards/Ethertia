cmake_minimum_required(VERSION 3.9)
project(Msgpack LANGUAGES CXX VERSION 1.0)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

add_library(Msgpack INTERFACE)
add_library(Msgpack::Msgpack ALIAS Msgpack)

target_compile_features(Msgpack INTERFACE cxx_std_17)

target_include_directories(Msgpack
                           INTERFACE
                           $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           )

install(TARGETS Msgpack
        EXPORT MsgpackTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(MSGPACK_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Msgpack")

install(EXPORT MsgpackTargets
        FILE MsgpackTargets.cmake
        NAMESPACE Msgpack::
        DESTINATION ${MSGPACK_CMAKE_CONFIG_DESTINATION})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/MsgpackConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/MsgpackConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/MsgpackConfig.cmake
                              INSTALL_DESTINATION ${MSGPACK_CMAKE_CONFIG_DESTINATION}
                              )

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/MsgpackConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/MsgpackConfigVersion.cmake
        DESTINATION ${MSGPACK_CMAKE_CONFIG_DESTINATION})

export(EXPORT MsgpackTargets FILE ${CMAKE_CURRENT_BINARY_DIR}/MsgpackTargets.cmake NAMESPACE Msgpack::)
export(PACKAGE Msgpack)

add_subdirectory(tests)