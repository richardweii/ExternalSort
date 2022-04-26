cmake_minimum_required(VERSION 3.16)
project(extsort)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

aux_source_directory(src SORT_SRCS)
aux_source_directory(src/io IO_SRCS)
aux_source_directory(src/util UTIL_SRCS)

set(
        SRCS
        ${SORT_SRCS}
        ${IO_SRCS}
)

message("Sources files:" ${SRCS})

add_executable(
        buffer_test
        test/buffer_test.cpp
        ${SRCS}
)

target_link_libraries(
        buffer_test
        gtest_main
)

add_executable(
        optional_test
        test/optional_test.cpp
        ${SRCS}
)

target_link_libraries(
        optional_test
        gtest_main
)

add_executable(
        buffer_iter_test
        test/buffer_iter_test.cpp
        ${SRCS}
)

target_link_libraries(
        buffer_iter_test
        gtest_main
)

add_executable(
        file_test
        test/file_test.cpp
        ${SRCS}
)

target_link_libraries(
        file_test
        gtest_main
)

add_executable(
        file_iter_test
        test/file_iterator_test.cpp
        ${SRCS}
)

target_link_libraries(
        file_iter_test
        gtest_main
)

add_executable(
        two_level_iter_test
        test/two_level_iter_test.cpp
        ${SRCS}
)

target_link_libraries(
        two_level_iter_test
        gtest_main
)

add_executable(
        sort_test
        test/sort_test.cpp
        ${SRCS}
)

target_link_libraries(
        sort_test
        gtest_main
)