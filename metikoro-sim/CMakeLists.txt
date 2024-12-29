cmake_minimum_required(VERSION 3.22)
add_executable(metikoro-sim src/main.cpp
        src/Application.hpp
        src/Console.hpp
        src/Configuration.hpp)
target_link_libraries(metikoro-sim PRIVATE metikoro-lib metikoro-sqlite sqlite3)
target_include_directories(metikoro-sim PRIVATE ../metikoro-lib/src ../metikoro-sqlite/src ../sqlite3)
target_compile_options(metikoro-sim PRIVATE -Wall -Wextra)
if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(metikoro-sim PRIVATE -O3)
endif ()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -O0")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
