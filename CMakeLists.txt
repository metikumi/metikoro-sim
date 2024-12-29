cmake_minimum_required(VERSION 3.22)
project(
        MetiKoroSimulation
        VERSION 1.0.0
        DESCRIPTION "A statistical simulation of the MetiKoro game.")
set(CMAKE_CXX_STANDARD 26)
add_subdirectory(metikoro-lib)
add_subdirectory(metikoro-sqlite)
add_subdirectory(metikoro-sim)
add_subdirectory(erbsland-unittest)
add_subdirectory(unittest)
