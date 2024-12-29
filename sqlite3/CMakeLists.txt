cmake_minimum_required(VERSION 3.22)
add_library(sqlite3
        sqlite3.c
        sqlite3.h
        sqlite3ext.h)
if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(sqlite3 PRIVATE -O3)
endif ()
