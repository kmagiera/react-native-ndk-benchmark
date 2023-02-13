cmake_minimum_required(VERSION 3.4.1)

set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)

add_library(
        benchmark-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/benchmark-lib.cpp)


set( lib_src_DIR src/main/cpp/benchmark )

set( lib_build_DIR ${lib_src_DIR}/build )
file(MAKE_DIRECTORY ${lib_build_DIR})


add_subdirectory(src/main/cpp/benchmark)
include_directories(src/main/cpp/benchmark/include)

find_library( log-lib log )

target_link_libraries( benchmark-lib benchmark ${log-lib})
