cmake_minimum_required(VERSION 3.22)
add_library(metikoro-sqlite
        src/SQLiteBackend.hpp
        src/SQLiteBackend.cpp
)
target_compile_options(metikoro-sqlite PRIVATE -Wall -Wextra)
target_include_directories(metikoro-sqlite PRIVATE ../metikoro-lib/src ../sqlite3)
if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(metikoro-sqlite PRIVATE -O3)
endif ()
