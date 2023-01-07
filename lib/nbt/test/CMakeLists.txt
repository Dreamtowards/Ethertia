enable_testing()
find_package(CxxTest REQUIRED)

include_directories(${libnbt++_SOURCE_DIR}/include)
include_directories(${CXXTEST_INCLUDE_DIR})

#Specifies that the directory containing the testfiles get copied when the target is built
function(use_testfiles target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/testfiles ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

if(NBT_USE_ZLIB)
    set(EXTRA_TEST_LIBS ${ZLIB_LIBRARY})
endif()

CXXTEST_ADD_TEST(nbttest nbttest.cpp ${CMAKE_CURRENT_SOURCE_DIR}/nbttest.h)
target_link_libraries(nbttest nbt++)

CXXTEST_ADD_TEST(endian_str_test endian_str_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/endian_str_test.h)
target_link_libraries(endian_str_test nbt++)

CXXTEST_ADD_TEST(read_test read_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/read_test.h)
target_link_libraries(read_test nbt++ ${EXTRA_TEST_LIBS})
use_testfiles(read_test)

CXXTEST_ADD_TEST(write_test write_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/write_test.h)
target_link_libraries(write_test nbt++ ${EXTRA_TEST_LIBS})
use_testfiles(write_test)

if(NBT_USE_ZLIB)
    CXXTEST_ADD_TEST(zlibstream_test zlibstream_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/zlibstream_test.h)
    target_link_libraries(zlibstream_test nbt++ ${EXTRA_TEST_LIBS})
    use_testfiles(zlibstream_test)
endif()

add_executable(format_test format_test.cpp)
target_link_libraries(format_test nbt++)
add_test(format_test format_test)
