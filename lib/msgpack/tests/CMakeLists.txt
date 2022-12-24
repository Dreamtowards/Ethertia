cmake_minimum_required(VERSION 3.9)

find_package(Catch2 REQUIRED)

add_executable(Msgpack_tests
               main.cpp
               type_packing_tests.cpp
               examples.cpp
               error_handling.cpp
               object_packing_tests.cpp
               )

if (MSVC)
    target_compile_options(Msgpack_tests PRIVATE /WX /Zc:__cplusplus)
else (MSVC)
    target_compile_options(Msgpack_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif (MSVC)

target_link_libraries(Msgpack_tests
                      Catch2::Catch2
                      Msgpack::Msgpack)

set_target_properties(Msgpack_tests PROPERTIES CXX_STANDARD 17)
target_compile_features(Msgpack_tests PUBLIC cxx_std_17)
